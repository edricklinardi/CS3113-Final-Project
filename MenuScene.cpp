/**
* Author: Edrick Linardi
* Assignment: Space Blaster
* Date due: 2025-05-02, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "MenuScene.h"
#include "Utility.h"

void MenuScene::initialise() {
    m_game_state.next_scene_id = -1;  // No transition yet

    Mix_HaltMusic();

    m_font_texture_id = Utility::load_texture("assets/font_sprite.png");
}

void MenuScene::update(float delta_time) {
}

void MenuScene::render(ShaderProgram* program) {
    glClear(GL_COLOR_BUFFER_BIT);

    Utility::draw_text(program, m_font_texture_id, "Space Blaster", 0.65f, 0.01f, glm::vec3(-3.75f, 0.0f, 0));
    Utility::draw_text(program, m_font_texture_id, "Press ENTER to start", 0.35f, 0.01f, glm::vec3(-3.25f, -0.5f, 0));
}