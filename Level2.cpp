/**
* Author: Edrick Linardi
* Assignment: Space Blaster
* Date due: 2025-05-02, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Level2.h"
#include "Utility.h"

extern ShaderProgram g_shader_program;

constexpr char SHIP_SPRITE_FILEPATH[] = "assets/Player/Ship.png";
constexpr char BOOSTER_SPRITE_FILEPATH[] = "assets/Player/Booster.png";
constexpr char BEAM_SPRITE_FILEPATH[] = "assets/Player/Beam.png";
constexpr char WALKER_SPRITE_FILEPATH[] = "assets/AI/Walker.png";
constexpr char GUARD_SPRITE_FILEPATH[] = "assets/AI/Guard.png";
constexpr char FLYER_SPRITE_FILEPATH[] = "assets/AI/Flyer.png";
constexpr char BACKGROUND_FILEPATH[] = "assets/Space BG.png";

constexpr char BGM_FILEPATH[] = "assets/Audio/bgm.mp3";
constexpr char DEATH_SFX_FILEPATH[] = "assets/Audio/player-death.wav";
constexpr char AI_DEATH_SFX_FILEPATH[] = "assets/Audio/ai-death.wav";
constexpr char BEAM_SFX_FILEPATH[] = "assets/Audio/laser-beam.wav";

extern int lives;

Level2Scene::~Level2Scene()
{
    delete[] m_game_state.enemies;
    delete m_game_state.player;

    Mix_FreeChunk(m_game_state.beam_sfx);
    Mix_FreeChunk(m_game_state.death_sfx);
    Mix_FreeChunk(m_game_state.ai_death_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void Level2Scene::initialise()
{
    m_game_state.next_scene_id = -1;

    // Background setup
    GLuint background_texture_id = Utility::load_texture(BACKGROUND_FILEPATH);
    m_game_state.bg_texture_id = background_texture_id;

    // Player setup
    GLuint ship_texture_id = Utility::load_texture(SHIP_SPRITE_FILEPATH);
    GLuint booster_texture_id = Utility::load_texture(BOOSTER_SPRITE_FILEPATH);
    GLuint beam_texture_id = Utility::load_texture(BEAM_SPRITE_FILEPATH);

    static int ship_animation_indices[] = { 0, 1, 2 };
    static int beam_animation_indices[] = { 0, 1, 2, 3 };

    m_game_state.player = new Entity(
        ship_texture_id,            // texture id
        4.0f,                       // speed
        1.0f,                       // width
        1.0f,                       // height
        PLAYER                      // entity type
    );

    m_game_state.player->set_position(glm::vec3(0.0f, -1.5f, 0.0f));
    m_game_state.player->set_scale(glm::vec3(1.0f, 1.0f, 0.0f));
    m_game_state.player->set_animation_indices(ship_animation_indices);
    m_game_state.player->set_animation_cols(3);
    m_game_state.player->set_animation_rows(1);
    m_game_state.player->set_animation_frames(3);
    m_game_state.player->set_animation_index(0);
    m_game_state.player->set_is_ship(true);
    m_game_state.player->set_booster_texture_id(booster_texture_id);

    m_game_state.player->set_lives(lives);

    m_game_state.beam = new Entity(
        beam_texture_id,                // texture id
        4.0f,                           // speed
        1.0f,                           // width
        1.0f,                           // height
        PROJECTILE                      // entity type
    );

    m_game_state.beam->set_animation_indices(beam_animation_indices);
    m_game_state.beam->set_scale(glm::vec3(0.5f, 1.0f, 0.0f));
    m_game_state.beam->set_animation_cols(4);
    m_game_state.beam->set_animation_rows(1);
    m_game_state.beam->set_animation_frames(4);
    m_game_state.beam->set_animation_index(0);
    m_game_state.beam->set_animation_time(0.0f);
    m_game_state.beam->deactivate();

    // Enemies setup
    GLuint walker_texture_id = Utility::load_texture(WALKER_SPRITE_FILEPATH);
    GLuint guard_texture_id = Utility::load_texture(GUARD_SPRITE_FILEPATH);
    GLuint flyer_texture_id = Utility::load_texture(FLYER_SPRITE_FILEPATH);

    static int walker_animation_indices[] = { 0, 1, 2, 3, 4, 5 };
    static int guard_animation_indices[] = { 0, 1, 2, 3 };
    static int flyer_animation_indices[] = { 0, 1, 2, 3, 4 };

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i] = Entity(guard_texture_id, 4.0f, 1.0f, 1.0f, ENEMY, GUARD, IDLE);
        m_game_state.enemies[i].activate();
    }

    // Walker AI
    glm::vec3 walker_positions[] = {
        glm::vec3(-4.0f, -2.5f, 0.0f),
        glm::vec3(3.0f, 2.0f, 0.0f),
        glm::vec3(5.5f, -1.0f, 0.0f)
    };
    for (int i = 0; i < 3; i++) {
        m_game_state.enemies[i].set_texture_id(walker_texture_id);
        m_game_state.enemies[i].set_ai_type(WALKER);
        m_game_state.enemies[i].set_position(walker_positions[i]);
        m_game_state.enemies[i].set_scale(glm::vec3(1.0f));
        m_game_state.enemies[i].set_speed(4.0f);
        m_game_state.enemies[i].set_animation_indices(walker_animation_indices);
        m_game_state.enemies[i].set_animation_cols(6);
        m_game_state.enemies[i].set_animation_rows(1);
        m_game_state.enemies[i].set_animation_frames(6);
        m_game_state.enemies[i].set_animation_index(0);
        m_game_state.enemies[i].set_animation_time(0.0f);
    }

    // Guard AI
    glm::vec3 guard_positions[] = {
        glm::vec3(-3.0f, 2.5f, 0.0f),
        glm::vec3(3.0f, 1.0f, 0.0f)
    };
    for (int i = 0; i < 2; i++) {
        int idx = 3 + i;
        m_game_state.enemies[idx].set_texture_id(guard_texture_id);
        m_game_state.enemies[idx].set_ai_type(GUARD);
        m_game_state.enemies[idx].set_position(guard_positions[i]);
        m_game_state.enemies[idx].set_scale(glm::vec3(1.0f));
        m_game_state.enemies[idx].set_speed(2.5f);
        m_game_state.enemies[idx].set_animation_indices(guard_animation_indices);
        m_game_state.enemies[idx].set_animation_cols(4);
        m_game_state.enemies[idx].set_animation_rows(1);
        m_game_state.enemies[idx].set_animation_frames(4);
        m_game_state.enemies[idx].set_animation_index(0);
        m_game_state.enemies[idx].set_animation_time(0.0f);
    }

    // Chaser AI
    glm::vec3 chaser_positions[] = {
        glm::vec3(-6.0f, 3.5f, 0.0f),
        glm::vec3(6.0f, 2.0f, 0.0f)
    };

    for (int i = 0; i < 2; i++) {
        int idx = 5 + i;
        m_game_state.enemies[idx].set_texture_id(flyer_texture_id);
        m_game_state.enemies[idx].set_ai_type(CHASER);
        m_game_state.enemies[idx].set_position(chaser_positions[i]);
        m_game_state.enemies[idx].set_scale(glm::vec3(1.0f));
        m_game_state.enemies[idx].set_speed(3.0f);
        m_game_state.enemies[idx].set_animation_indices(flyer_animation_indices);
        m_game_state.enemies[idx].set_animation_cols(5);
        m_game_state.enemies[idx].set_animation_rows(1);
        m_game_state.enemies[idx].set_animation_frames(5);
        m_game_state.enemies[idx].set_animation_index(0);
        m_game_state.enemies[idx].set_animation_time(0.0f);
    }

    // Zigzag AI
    m_game_state.enemies[7].set_texture_id(flyer_texture_id);
    m_game_state.enemies[7].set_ai_type(ZIGZAG);
    m_game_state.enemies[7].set_position(glm::vec3(0.0f, 3.0f, 0.0f));
    m_game_state.enemies[7].set_scale(glm::vec3(1.0f));
    m_game_state.enemies[7].set_speed(3.5f);
    m_game_state.enemies[7].set_animation_indices(flyer_animation_indices);
    m_game_state.enemies[7].set_animation_cols(5);
    m_game_state.enemies[7].set_animation_rows(1);
    m_game_state.enemies[7].set_animation_frames(5);
    m_game_state.enemies[7].set_animation_index(0);
    m_game_state.enemies[7].set_animation_time(0.0f);


    // Audio setup
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_game_state.bgm = Mix_LoadMUS(BGM_FILEPATH);
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(25);

    m_game_state.beam_sfx = Mix_LoadWAV(BEAM_SFX_FILEPATH);
    m_game_state.death_sfx = Mix_LoadWAV(DEATH_SFX_FILEPATH);
    m_game_state.ai_death_sfx = Mix_LoadWAV(AI_DEATH_SFX_FILEPATH);

    Mix_VolumeChunk(m_game_state.ai_death_sfx, MIX_MAX_VOLUME);
}

void Level2Scene::update(float delta_time)
{
    m_game_state.player->update(delta_time, nullptr, nullptr, 0, nullptr);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, nullptr);

        if (m_game_state.enemies[i].is_active() &&
            m_game_state.player->is_active() &&
            m_game_state.enemies[i].check_collision(m_game_state.player))
        {
            Mix_PlayChannel(-1, m_game_state.death_sfx, 0); // Play player death sound

            m_game_state.player->lose_life();
            lives = m_game_state.player->get_lives();

            if (lives <= 0)
            {
                m_game_state.player->deactivate();
                m_game_state.next_scene_id = 5; // LoseScene
                return;
            }

            // Restarts level if player dies
            initialise();
            m_game_state.player->set_lives(lives);
        }
    }

    if (m_game_state.beam->is_active())
    {
        m_game_state.beam->update(delta_time, nullptr, nullptr, 0, nullptr);

        glm::vec3 camera_center = m_game_state.player->get_position();
        float upper_bound = camera_center.y + 3.75f;
        float lower_bound = camera_center.y - 3.75f;

        float beam_y = m_game_state.beam->get_position().y;

        if (beam_y > upper_bound || beam_y < lower_bound)
        {
            m_game_state.beam->deactivate();
        }

        for (int i = 0; i < ENEMY_COUNT; i++)
        {
            if (m_game_state.enemies[i].is_active() &&
                m_game_state.beam->check_collision(&m_game_state.enemies[i]))
            {
                Mix_PlayChannel(1, m_game_state.ai_death_sfx, 0);   // Play enemy death sound
                m_game_state.enemies[i].deactivate();               // Kill enemy
                m_game_state.beam->deactivate();                    // Remove beam
                break;                                              // Avoid double-deactivation
            }
        }
    }

    bool all_enemies_inactive = true;
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        if (m_game_state.enemies[i].is_active())
        {
            all_enemies_inactive = false;
            break;
        }
    }

    if (all_enemies_inactive)
    {
        m_game_state.next_scene_id = 3; // Level 3
    }
}

void Level2Scene::render(ShaderProgram* program)
{
    GLuint shader_program_id = program->get_program_id();
    glUseProgram(shader_program_id);
    GLint tint_location = glGetUniformLocation(shader_program_id, "healthTintAmount");

    glUniform1f(tint_location, 0.0f);
    Utility::draw_background(program, m_game_state.bg_texture_id, 50.0f, 50.0f);


    float tint = 1.0f - (static_cast<float>(lives) / 3.0f);  // Adjust denominator if max lives ≠ 3

    glUniform1f(tint_location, tint);

    m_game_state.player->render(program);

    if (m_game_state.beam->is_active())
    {
        m_game_state.beam->render(program);
    }

    glUniform1f(tint_location, 0.0f);
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        if (m_game_state.enemies[i].is_active())
        {
            m_game_state.enemies[i].render(program);
        }
    }
}