#include "utils.h"
#include "math.h"
#include <tuple>
#include <vector>
#include <algorithm>

#define DRAW_CUBIC_BEZIER 0 // Use to switch Linear and Cubic bezier curves
#define SAMPLES_PER_BEZIER 30 //Sample each Bezier curve as N=10 segments and draw as connected lines

// GLobal variables
std::vector<float> controlPoints;
std::vector<float> linearBezier;
std::vector<float> pointsLoc;
std::vector<std::tuple<float,float>> placePoints;
std::vector<float> cubicBezier;
std::tuple<float,float> selectedCoords;
bool pointSelected = false;
bool pointBuffer = false;
int points=0;
int width = 640, height = 640; 
bool controlPointsUpdated = false;
bool linearBezDraw = true;

float dist(int x1, int y1, int x2, int y2)
{
    return sqrt(abs(pow(x2 - x1, 2)) + abs(pow(y2 - y1, 2)));
}

void calculatePiecewiseLinearBezier()
{
    // Since linearBezier is just a polyline, we can just copy the control points and plot.
    // However to show how a piecewise parametric curve needs to be plotted, we sample t and
    // evaluate all linear bezier curves.
//    linearBezier.assign(controlPoints.begin(), controlPoints.end());

    linearBezier.clear();
    int sz  = controlPoints.size(); // Contains 3 points/vertex. Ignore Z
    float x[2], y[2];
    float delta_t = 1.0/(SAMPLES_PER_BEZIER - 1.0);
    float t;
    for(int i=0; i<(sz-3); i+=3) {
        x[0] = controlPoints[i];
        y[0] = controlPoints[i+1];
        x[1] = controlPoints[i+3];
        y[1] = controlPoints[i+4];
        linearBezier.push_back(x[0]);
        linearBezier.push_back(y[0]);
        linearBezier.push_back(0.0);
        t = 0.0;
        for (float j=1; j<(SAMPLES_PER_BEZIER-1); j++)
        {
        t += delta_t;
        linearBezier.push_back(x[0] + t*(x[1] - x[0]));
        linearBezier.push_back(y[0] + t*(y[1] - y[0]));
        linearBezier.push_back(0.0);
        }
        // No need to add the last point for this segment, since it will be added as first point in next.
    }
    // However, add last point of entire piecewise curve here (i.e, the last control point)
    linearBezier.push_back(x[1]);
    linearBezier.push_back(y[1]);
    linearBezier.push_back(0.0);
}

void calculatePiecewiseCubicBezier(bool endpoint)
{

    if(endpoint) {
        std::cout << "Entered Cubic Bezier " << " " << points << std::endl;
        if(linearBezDraw==false){
            cubicBezier.clear();
        }
        int sz = controlPoints.size(); // Contains 3 points/vertex. Ignore Z
        float x[4], y[4];
        float delta_t = 1.0 / (SAMPLES_PER_BEZIER - 1.0);
        float t;

        int i = controlPoints.size() - 15;

        x[0] = controlPoints[i];
        y[0] = controlPoints[i + 1];
        x[1] = controlPoints[i + 3];
        y[1] = controlPoints[i + 4];

        x[2] = controlPoints[i + 6];
        y[2] = controlPoints[i + 7];
        x[3] = controlPoints[i + 9];
        y[3] = controlPoints[i + 10];

        cubicBezier.push_back(x[0]);
        cubicBezier.push_back(y[0]);
        cubicBezier.push_back(0.0);
        t = 0.0;
        for (float j = 1; j < (SAMPLES_PER_BEZIER); j++) {
            t += delta_t;
            cubicBezier.push_back(
                    (x[0] * pow((1 - t), 3)) + (3 * t * x[1] * pow((1 - t), 2)) + (3 * (1 - t) * x[2] * pow(t, 2)) +
                    (x[3] * pow(t, 3)));
            cubicBezier.push_back(
                    (y[0] * pow((1 - t), 3)) + (3 * t * y[1] * pow((1 - t), 2)) + (3 * (1 - t) * y[2] * pow(t, 2)) +
                    (y[3] * pow(t, 3)));
            cubicBezier.push_back(0.0);
        }

        cubicBezier.push_back(x[3]);
        cubicBezier.push_back(y[3]);
        cubicBezier.push_back(0.0);
        linearBezDraw = true;
    }
}

int main(int, char* argv[])
{
    GLFWwindow* window = setupWindow(width, height);
    ImGuiIO& io = ImGui::GetIO(); // Create IO object

    ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    unsigned int shaderProgram = createProgram("./shaders/vshader.vs", "./shaders/fshader.fs");
	glUseProgram(shaderProgram);

    // Create VBOs, VAOs
    unsigned int VBO_controlPoints, VBO_linearBezier, VBO_cubicBezier;
    unsigned int VAO_controlPoints, VAO_linearBezier, VAO_cubicBezier;
    glGenBuffers(1, &VBO_controlPoints);
    glGenVertexArrays(1, &VAO_controlPoints);
    glGenBuffers(1, &VBO_linearBezier);
    glGenVertexArrays(1, &VAO_linearBezier);

    //TODO:
    glGenBuffers(2, &VBO_cubicBezier);
    glGenVertexArrays(2, &VAO_cubicBezier);


    int button_status = 0;

    //Display loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Rendering
        showOptionsDialog(controlPoints, io);
        ImGui::Render();

        // Add a new point on mouse click
        float x,y ;
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        bool endpoint = false;
        if(io.MouseClicked[0] && !ImGui::IsAnyItemActive()){

            x = io.MousePos.x;
            y = io.MousePos.y;

            if (pointSelected && pointBuffer)
            {
                float selected_x = std::get<0>(selectedCoords);
                float selected_y = std::get<1>(selectedCoords);
//                std::cout<< selected_x << selected_y << "hree" << std::endl;

                float rescaled_x1 = -1.0 + ((1.0*selected_x - 0) / (width - 0)) * (1.0 - (-1.0));
                float rescaled_y1 = -1.0 + ((1.0*(height - selected_y) - 0) / (height - 0)) * (1.0 - (-1.0));

                float rescaled_x2 = -1.0 + ((1.0*x - 0) / (width - 0)) * (1.0 - (-1.0));
                float rescaled_y2 = -1.0 + ((1.0*(height - y) - 0) / (height - 0)) * (1.0 - (-1.0));
                auto it = std::find (controlPoints.begin(), controlPoints.end(), rescaled_x1);
                if (it != controlPoints.end())
                {

                    controlPoints.at(it-controlPoints.begin()) = rescaled_x2;
                    controlPoints.at(it-controlPoints.begin() + 1) = rescaled_y2;
                    controlPointsUpdated = true;
                    pointSelected = false;
                    pointBuffer = false;
                    placePoints.emplace_back(x,y);
                    std::cout << "A point has been changed." << std::endl;
                    endpoint = true;
                    linearBezDraw = false;
                }


            }

            else{
                for ( const auto& i : placePoints ) {
                    float diff_x = abs(std::get<0>(i) - x);
                    float diff_y = abs(std::get<1>(i) - y);
                    float threshold = 30;
//                    std::cout << diff_x << diff_y << std::endl;
                    if(diff_x<=threshold && diff_y<=threshold)
                    {
                        std::cout << "Point exists." << std::endl;
                        pointSelected = true;
                        std::get<0>(selectedCoords) = std::get<0>(i);
                        std::get<1>(selectedCoords) = std::get<1>(i);
                    }
                    else{
//                        std::cout << "NOT Found" << std::endl;
                        placePoints.emplace_back(x,y);
                    }
                }

                if(pointSelected)
                {
                    float selected_x = std::get<0>(selectedCoords);
                    float selected_y = std::get<1>(selectedCoords);

                    float rescaled_x = -1.0 + ((1.0*selected_x - 0) / (width - 0)) * (1.0 - (-1.0));
                    float rescaled_y = -1.0 + ((1.0*(height - selected_y) - 0) / (height - 0)) * (1.0 - (-1.0));
                    pointBuffer = true;

                }
                else{
                    if(placePoints.empty())
                    {
                        placePoints.emplace_back(x,y);
                    }

                    pointsLoc.push_back(x);
                    pointsLoc.push_back(y);
                    points++;
                    addPoints(controlPoints, x, y, width, height);
                    controlPointsUpdated = true;

                    if((points%3==1 && points>4) || points==4) {
                        endpoint = true;
                        int pS = pointsLoc.size();
                        float col_x = (2 * pointsLoc[pS - 2]) - pointsLoc[pS - 4];
                        float col_y = (2 * pointsLoc[pS - 1]) - pointsLoc[pS - 3];
                        points++;
                        addPoints(controlPoints, col_x, col_y, width, height);
                        controlPointsUpdated = true;
                    }
                }
            }

         }

        if(controlPointsUpdated) {
            // Update VAO/VBO for control points (since we added a new point)
            glBindVertexArray(VAO_controlPoints);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_controlPoints);
            glBufferData(GL_ARRAY_BUFFER, controlPoints.size()*sizeof(GLfloat), &controlPoints[0], GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0); //Enable first attribute buffer (vertices)

            // Update VAO/VBO for piecewise linear Bezier curve (since we added a new point)
            if(linearBezDraw){
                calculatePiecewiseLinearBezier();
            }

            glBindVertexArray(VAO_linearBezier);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_linearBezier);
            glBufferData(GL_ARRAY_BUFFER, linearBezier.size()*sizeof(GLfloat), &linearBezier[0], GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0); //Enable first attribute buffer (vertices)

            // Update VAO/VBO for piecewise cubic Bezier curve
            // TODO:
            calculatePiecewiseCubicBezier(endpoint);
            glBindVertexArray(VAO_cubicBezier);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_cubicBezier);
            glBufferData(GL_ARRAY_BUFFER, cubicBezier.size()*sizeof(GLfloat), &cubicBezier[0], GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0); //Enable first attribute buffer (vertices)
            controlPointsUpdated = false; // Finish all VAO/VBO updates before setting this to false.
        }

        glUseProgram(shaderProgram);

        // Draw control point\s
        glBindVertexArray(VAO_controlPoints);
		glDrawArrays(GL_POINTS, 0, controlPoints.size()/3); // Draw points


#if DRAW_CUBIC_BEZIER
        // TODO:
#else
        // Draw linear Bezier
        glBindVertexArray(VAO_linearBezier);
        glDrawArrays(GL_LINE_STRIP, 0, linearBezier.size()/3); //Draw lines

        glBindVertexArray(VAO_cubicBezier);
        glDrawArrays(GL_LINE_STRIP, 0, cubicBezier.size()/3); //Draw lines
#endif

        glUseProgram(0);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);


    }

    // Delete VBO buffers
    glDeleteBuffers(1, &VBO_controlPoints);
    glDeleteBuffers(1, &VBO_linearBezier);
    glDeleteBuffers(1, &VBO_cubicBezier);
    //TODO:

    // Cleanup
    cleanup(window);
    return 0;
}
