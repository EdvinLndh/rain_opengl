#include "imgui.h"


void imgui_init(GLFWwindow *window, const char *glsl_version)
{
    igCreateContext(NULL);
    ImGuiIO *ioptr = igGetIO();
    ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

#ifdef IMGUI_HAS_DOCK
    ioptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
    ioptr->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
#endif

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    igStyleColorsDark(NULL);
}

void imgui_start_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();
}

void imgui_render(GLFWwindow *window)
{
    ImGuiIO *ioptr = igGetIO();
    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());

#ifdef IMGUI_HAS_DOCK
    if (ioptr->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow *backup_current_window = glfwGetCurrentContext();
        igUpdatePlatformWindows();
        igRenderPlatformWindowsDefault(NULL, NULL);
        glfwMakeContextCurrent(backup_current_window);
    }
#endif
    glViewport(0, 0, (int)ioptr->DisplaySize.x, (int)ioptr->DisplaySize.y);
}

void imgui_button(const char *label, float x, float y, void *data, ButtonCallback callback)
{
    if (igButton(label, (ImVec2){x, y}))
    {
        callback(data);
    }
}

void imgui_cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(NULL);
}

void imgui_treenode_particle(char *title, int num_particles, float rainx, float rainy, float rainz, vec4s color, Shader shader, ButtonCallback func) {
       if (igTreeNode_Str(title))
        {

            igInputInt("# Particles", &num_particles, 10, 100, 0);
            igSliderFloat("x max", &rainx, 1, 100, "%.3f", 0);
            igSliderFloat("y max", &rainy, 1, 100, "%.3f", 0);
            igSliderFloat("z max", &rainz, 1, 100, "%.3f", 0);

            igColorEdit4("Color", (float *)&color, 0);
            ParticleConfig *config = malloc(sizeof(ParticleConfig));
            if (config == NULL)
            {
                fprintf(stderr, "Can't malloc ParticleConfig\n");
                exit(EXIT_FAILURE);
            }
            *config = (ParticleConfig){
                .num_particles = num_particles,
                .x = rainx,
                .y = rainy,
                .z = rainz,
                .color = color,
                .shader = shader,
                .type = RAIN,
            };

            imgui_button("Add rain", 60.0f, 25.0f, (void *)config, func);

            igTreePop();
        }

}

