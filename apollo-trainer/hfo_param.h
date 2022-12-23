#ifndef HFO_PARAM_H
#define HFO_PARAM_H

#include <string>

class HFOParam {
 public:
  HFOParam();
  ~HFOParam() {}

  HFOParam& instance() {
    static HFOParam M_hfo_param;
    return M_hfo_param;
  }
  HFOParam& i() { return instance(); }

 private:
  HFOParam(const HFOParam&);
  HFOParam& operator=(const HFOParam&);

  // HFO parameter
 public:
  // player number
  int M_hfo_offense_player;
  int M_hfo_defense_player;

  // msg
  static const char* oobMsg;
  static const char* capturedMsg;
  static const char* goalMsg;
  static const char* ootMsg;
  static const char* doneMsg;
  static const char* inGameMsg;
  static const int TURNOVER_TIME;

  // log
  static const int HFO_LOGGING;
  static const std::string HFO_LOG_DIR;
  static const std::string HFO_LOG_FIXED_NAME;
  static const int HFO_LOG_FIXED;
  static const int HFO_LOG_DATED;

  bool M_hfo_logging;
  std::string M_hfo_log_dir;
  std::string M_hfo_log_fixed_name;
  bool M_hfo_log_fixed;
  bool M_hfo_log_dated;

  // time && position
  // bool   M_hfo;                    /* HFO mode on/off */
  int M_hfo_max_trial_time;     /* Max time an HFO trial can last */
  int M_hfo_max_untouched_time; /* Max time ball can go untouched in HFO */
  int M_hfo_max_trials;         /* Quit after this many HFO trials */
  int M_hfo_max_frames;         /* Quit after this many HFO frames */
  int M_hfo_offense_on_ball;    /* Give the ball to an offensive player */
  double M_hfo_min_ball_pos_x;  /* Governs the initialization x-position of
                                   ball */
  double M_hfo_max_ball_pos_x;  /* Governs the initialization x-position of
                                   ball */
  double M_hfo_min_ball_pos_y;  /* Governs the initialization y-position of
                                   ball */
  double M_hfo_max_ball_pos_y;  /* Governs the initialization y-position of
                                   ball */
  double M_hfo_min_ball_vel_x;
  double M_hfo_max_ball_vel_x;
  double M_hfo_min_ball_vel_y;
  double M_hfo_max_ball_vel_y;

  double M_hfo_min_player_pos_x;
  double M_hfo_max_player_pos_x;
  double M_hfo_min_player_pos_y;
  double M_hfo_max_player_pos_y;
  //   double M_hfo_min_player_vel_x;
  //   double M_hfo_max_player_vel_x;
  //   double M_hfo_min_player_vel_y;
  //   double M_hfo_max_player_vel_y;
};

#endif