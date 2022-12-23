#include "hfo_param.h"

// HFO parameters
const int HFOParam::HFO_LOGGING = true;
#ifdef RCSS_WIN
const std::string HFOParam::HFO_LOG_DIR = ".\\";
#else
const std::string HFOParam::HFO_LOG_DIR = "./";
#endif
const std::string HFOParam::HFO_LOG_FIXED_NAME = "rcssserver";
const int HFOParam::HFO_LOG_FIXED = false;
const int HFOParam::HFO_LOG_DATED = true;

const char* HFOParam::oobMsg = "OUT_OF_BOUNDS";
const char* HFOParam::capturedMsg = "CAPTURED_BY_DEFENSE";
const char* HFOParam::goalMsg = "GOAL";
const char* HFOParam::ootMsg = "OUT_OF_TIME";
const char* HFOParam::doneMsg = "HFO_FINISHED";
const char* HFOParam::inGameMsg = "IN_GAME";
const int HFOParam::TURNOVER_TIME = 2;

HFOParam::HFOParam()
    : M_hfo_offense_player(1),
      M_hfo_defense_player(0),
      M_hfo_max_trial_time(100),
      M_hfo_max_untouched_time(100),
      M_hfo_max_trials(-1),
      M_hfo_max_frames(-1),
      M_hfo_offense_on_ball(false),

      // 0.0 ~ 1.0
      M_hfo_min_ball_pos_x(0),
      M_hfo_max_ball_pos_x(2),
      // -1.0 ~ 1.0
      M_hfo_min_ball_pos_y(0),
      M_hfo_max_ball_pos_y(0),

      M_hfo_min_ball_vel_x(0.0),
      M_hfo_max_ball_vel_x(0),
      M_hfo_min_ball_vel_y(0.0),
      M_hfo_max_ball_vel_y(0),

      M_hfo_min_player_pos_x(0.0),
      M_hfo_max_player_pos_x(0.0),
      M_hfo_min_player_pos_y(0.0),
      M_hfo_max_player_pos_y(0.0),

      M_hfo_logging(HFO_LOGGING),
      M_hfo_log_dir(HFO_LOG_DIR),
      M_hfo_log_fixed_name(HFO_LOG_FIXED_NAME),
      M_hfo_log_fixed(HFO_LOG_FIXED),
      M_hfo_log_dated(HFO_LOG_DATED) {}
