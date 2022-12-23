// -*-c++-*-

/*
 *Copyright:

 Copyright (C) Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3, or (at your option)
 any later version.

 This code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this code; see the file COPYING.  If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rcsc/coach/coach_world_model.h>
#include <rcsc/common/abstract_client.h>
#include <rcsc/common/player_param.h>
#include <rcsc/common/player_type.h>
#include <rcsc/common/server_param.h>
#include <rcsc/param/cmd_line_parser.h>
#include <rcsc/param/param_map.h>
#include <rcsc/random.h>
#include <rcsc/trainer/trainer_command.h>
#include <rcsc/trainer/trainer_config.h>

#include "hfo_random.h"
#include "hfo_trainer.h"

using namespace rcsc;

// HFO status

/*-------------------------------------------------------------------*/
/*!

 */
HFOTrainer::HFOTrainer()
    : TrainerAgent(),
      M_episode(0),
      M_offense(0),
      M_defense(0),
      M_time(0),
      M_take_time(0),
      M_holder_unum(-1),
      M_holder_side('U'),
      M_prev_ball_pos(0.0, 0.0),
      M_untouched_time(0),
      M_episode_begin_time(-1),
      M_episode_over_time(-1),
      M_rng(),
      M_hfo_param() {
  // Generate vector of offsets used when resetting the field.
  float offset_x[10] = {-1, -1, 1, 1, 0, 0, -2, -2, 2, 2};
  float offset_y[10] = {-1, 1, 1, -1, 2, -2, -2, 2, 2, -2};
  for (int i = 0; i < 10; ++i) {
    M_offsets.push_back(std::make_pair(offset_x[i], offset_y[i]));
  }
}

/*-------------------------------------------------------------------*/
/*!

 */
HFOTrainer::~HFOTrainer() {}

/*-------------------------------------------------------------------*/
/*!

 */
bool HFOTrainer::initImpl(CmdLineParser& cmd_parser) {
  bool result = TrainerAgent::initImpl(cmd_parser);

#if 0
    ParamMap my_params;

    std::string formation_conf;
    my_map.add()
        ( &conf_path, "fconf" )
        ;

    cmd_parser.parse( my_params );
#endif
  ParamMap hfo_params;

  std::string tmp;
  hfo_params.add()("tmp", "t", &tmp, "123");
  // more parameter here
  hfo_params.add()("trials", "", &M_hfo_param.M_hfo_max_trials, "max trails");
  hfo_params.add()("frames", "", &M_hfo_param.M_hfo_max_frames, "max frames");
  hfo_params.add()("offense-agents", "on", &M_hfo_param.M_hfo_offense_player,
                   "number of offenseplayer");
  hfo_params.add()("defense-agents", "dn", &M_hfo_param.M_hfo_defense_player,
                   "number of defence players");
  hfo_params.add()("ball-pos-x", "", &M_hfo_param.M_hfo_max_ball_pos_x);
  hfo_params.add()("ball-pos-y", "", &M_hfo_param.M_hfo_max_ball_pos_y);
  hfo_params.add()("ball-vel-x", "", &M_hfo_param.M_hfo_max_ball_vel_x);
  hfo_params.add()("ball-vel-y", "", &M_hfo_param.M_hfo_max_ball_vel_y);
  hfo_params.add()("player-pos-x", "", &M_hfo_param.M_hfo_max_player_pos_x);
  hfo_params.add()("player-pos-y", "", &M_hfo_param.M_hfo_max_player_pos_y);
  cmd_parser.parse(hfo_params);
  // todo: more parameter

  if (cmd_parser.failed()) {
    std::cerr << "hfo trainer: ***WARNING*** detected unsupported options: ";
    cmd_parser.print(std::cerr);
    std::cerr << std::endl;
  }

  if (!result) {
    return false;
  }

  //////////////////////////////////////////////////////////////////
  // Add your code here.
  //////////////////////////////////////////////////////////////////

  return true;
}

/*-------------------------------------------------------------------*/
/*!

 */
void HFOTrainer::actionImpl() {
  if (world().teamNameLeft().empty()) {
    doTeamNames();
    return;
  }

  //////////////////////////////////////////////////////////////////
  // Add your code here.

  // sampleAction();
  // recoverForever();
  // doSubstitute();
  // doKeepaway();
  doHFO();
}

/*-------------------------------------------------------------------*/
/*!

*/
void HFOTrainer::handleInitMessage() {}

/*-------------------------------------------------------------------*/
/*!

*/
void HFOTrainer::handleServerParam() {}

/*-------------------------------------------------------------------*/
/*!

*/
void HFOTrainer::handlePlayerParam() {}

/*-------------------------------------------------------------------*/
/*!

*/
void HFOTrainer::handlePlayerType() {}

/*-------------------------------------------------------------------*/
/*!

 */
void HFOTrainer::sampleAction() {
  // sample training to test a ball interception.

  static int s_state = 0;
  static int s_wait_counter = 0;

  static Vector2D s_last_player_move_pos;

  if (world().existKickablePlayer()) {
    s_state = 1;
  }

  switch (s_state) {
    case 0:
      // nothing to do
      break;
    case 1:
      // exist kickable left player

      // recover stamina
      doRecover();
      // move ball to center
      doMoveBall(Vector2D(0.0, 0.0), Vector2D(0.0, 0.0));
      // change playmode to play_on
      doChangeMode(PM_PlayOn);
      {
        // move player to random point
        UniformReal uni01(0.0, 1.0);
        Vector2D move_pos = Vector2D::polar2vector(15.0,  // 20.0,
                                                   AngleDeg(360.0 * uni01()));
        s_last_player_move_pos = move_pos;

        doMovePlayer(config().teamName(),
                     1,  // uniform number
                     move_pos, move_pos.th() - 180.0);
      }
      // change player type
      {
        static int type = 0;
        doChangePlayerType(world().teamNameLeft(), 1, type);
        type = (type + 1) % PlayerParam::i().playerTypes();
      }

      doSay("move player");
      s_state = 2;
      std::cout << "trainer: actionImpl init episode." << std::endl;
      break;
    case 2:
      ++s_wait_counter;
      if (s_wait_counter > 3 && !world().playersLeft().empty()) {
        // add velocity to the ball
        // UniformReal uni_spd( 2.7, 3.0 );
        // UniformReal uni_spd( 2.5, 3.0 );
        UniformReal uni_spd(2.3, 2.7);
        // UniformReal uni_ang( -50.0, 50.0 );
        UniformReal uni_ang(-10.0, 10.0);
        Vector2D velocity = Vector2D::polar2vector(
            uni_spd(), s_last_player_move_pos.th() + uni_ang());
        doMoveBall(Vector2D(0.0, 0.0), velocity);
        s_state = 0;
        s_wait_counter = 0;
        std::cout << "trainer: actionImpl start ball" << std::endl;
      }
      break;
  }
}

/*-------------------------------------------------------------------*/
/*!

 */
void HFOTrainer::recoverForever() {
  if (world().playersLeft().empty()) {
    return;
  }

  if (world().time().stopped() == 0 && world().time().cycle() % 50 == 0) {
    // recover stamina
    doRecover();
    doTeamNames();
  }
}

/*-------------------------------------------------------------------*/
/*!

 */
void HFOTrainer::doSubstitute() {
  static bool s_substitute = false;
  if (!s_substitute && world().time().cycle() == 0 &&
      world().time().stopped() >= 10) {
    std::cerr << "trainer " << world().time()
              << " team name = " << world().teamNameLeft() << std::endl;

    if (!world().teamNameLeft().empty()) {
      UniformSmallInt uni(0, PlayerParam::i().ptMax());
      doChangePlayerType(world().teamNameLeft(), 1, uni());

      s_substitute = true;
    }
  }

  if (world().time().stopped() == 0 && world().time().cycle() % 100 == 1 &&
      !world().teamNameLeft().empty()) {
    static int type = 0;
    doChangePlayerType(world().teamNameLeft(), 1, type);
    type = (type + 1) % PlayerParam::i().playerTypes();
  }
}

/*-------------------------------------------------------------------*/
/*!

 */
void HFOTrainer::doKeepaway() {
  if (world().trainingTime() == world().time()) {
    std::cerr << "trainer: " << world().time() << " keepaway training time."
              << std::endl;
  }
}

/*-------------------------------------------------------------------*/
/*!
    callback function
 */
void HFOTrainer::doHFO() {
  // todo 开赛条件
  // 当前为，右边有两名球员在场时，开始HFO
  if (!M_episode &&
      (world().playersLeft().size() == M_hfo_param.M_hfo_offense_player) &&
      (world().playersRight().size() == M_hfo_param.M_hfo_defense_player)) {
    initHFO();
  }
  analyse();
}

/*-------------------------------------------------------------------*/
/*!
    for initialize parameter of HFO
 */
void HFOTrainer::initHFO() {
  if (M_episode == 0) {
    //生成本场比赛的随机种子（或使用指定的种子）
    // Seed the RNG needed to reset the field
    if (ServerParam::instance().randomSeed() >= 0) {
      int seed = ServerParam::instance().randomSeed();
      std::cout << "HFORef using seed: " << seed << std::endl;
      M_rng.seed(seed);
    } else {
      int seed = irand(RAND_MAX);
      std::cout << "HFORef using Random Seed: " << seed << std::endl;
      M_rng.seed(seed);
    }
    auto end = world().allPlayers().end();
    for (auto p = world().allPlayers().begin(); p != end; ++p) {
      if (!(*p)->isValid()) continue;

      if ((*p)->side() == LEFT) {
        ++M_offense;
      } else if ((*p)->side() == RIGHT) {
        ++M_defense;
      }
    }
    // logHeader();
    std::cout << "Begin HFO" << std::endl;
  }
  M_episode++;
  doChangeMode(PM_PlayOn);
  M_episode_over_time = 1;
}

/*-------------------------------------------------------------------*/
/*!
    core implementation of HFO
 */
void HFOTrainer::analyse() {
  const CoachWorldModel& wm = world();
  if ((M_hfo_param.M_hfo_max_trials > 0 &&
       M_episode > M_hfo_param.M_hfo_max_trials) ||
      (M_hfo_param.M_hfo_max_frames > 0 &&
       wm.time().cycle() > M_hfo_param.M_hfo_max_frames)) {
    //如果超出训练次数或训练时间，就停止训练
    finalize();
    return;
  }

  if (M_episode_over_time > 0) {
    //每次输出完本轮的log，重置本轮的状态
    if (wm.gameMode().type() == GameMode::PlayOn) {
      doChangeMode(PM_BeforeKickOff);
      resetField();
      std::cout << M_episode << "@"
                << M_episode_over_time - M_episode_begin_time << "@"
                << M_episode_begin_time << std::endl;
    }
    if (wm.time().stopped() > 20) {
      //等待10个周期后才开始比赛
      doChangeMode(PM_PlayOn);
      M_episode_over_time = 0;
      M_episode_begin_time = wm.time().cycle();
      ++M_episode;
      // todo 输出重置信息
    }
    return;
  }

  if (wm.gameMode().type() == GameMode::PlayOn) {
    //正常比赛模式下的判断
    if (wm.ball().pos().dist(wm.allPlayers().at(0)->pos()) <
        ServerParam::i().defaultKickableArea()) {
      //用于apollo2022校赛，表示球员接到了球
      M_episode_over_time = wm.time().cycle();
    }
    if (crossGoalLine(RIGHT, wm.ball().pos())) {
      //判断是否进球
      char gMsg[32];
      sprintf(gMsg, "%s-%d", M_hfo_param.goalMsg, M_holder_unum);
      M_episode_over_time = wm.time().cycle();
      // } else if (!(inHFOArea(wm.ball().pos()))) {
      //   //判断是否在HFO（规定的）范围内
      //   // oobMsg
      //   M_episode_over_time = wm.time().cycle();
    } else if (M_take_time >= M_hfo_param.TURNOVER_TIME) {
      //判断控球时间是否过长
      // capMsg
      char capMsg[32];
      sprintf(capMsg, "%s-%d", M_hfo_param.capturedMsg, M_holder_unum);
      M_episode_over_time = wm.time().cycle();
    } else if ((M_hfo_param.M_hfo_max_untouched_time > 0 &&
                M_untouched_time > M_hfo_param.M_hfo_max_untouched_time) ||
               (M_hfo_param.M_hfo_max_trial_time > 0 &&
                wm.time().cycle() - M_time >
                    M_hfo_param.M_hfo_max_trial_time)) {
      //判断是否超时
      // ootMsg
      M_episode_over_time = wm.time().cycle();
    } else {
      //判断是否丢失球权
      ++M_untouched_time;
      bool offense_poss = false;
      for (auto p = wm.allPlayers().begin(); p != wm.allPlayers().end(); ++p) {
        if (!(*p)->isValid()) continue;

        Vector2D ppos = (*p)->pos();

        if (ppos.dist2(wm.ball().pos()) <
            std::pow(ServerParam::i().defaultKickableArea(), 2)) {
          M_holder_unum = (*p)->unum();
          M_untouched_time = 0;
          if ((*p)->side() == LEFT) {
            offense_poss = true;
            M_holder_side = 'L';
          } else if ((*p)->side() == RIGHT) {
            offense_poss = false;
            M_holder_side = 'R';
            ++M_take_time;
            break;
          }
        }
      }
      if (offense_poss) {
        M_take_time = 0;
      }

      bool goalie_has_ball = (M_holder_side == 'R' && M_holder_unum == 1);
      if (goalie_has_ball) {
        // cap
        M_episode_over_time = wm.time().cycle();
      } else {
        // inGame
      }
    }
  }
  // 记录上一个cycle的状态
  M_prev_ball_pos = wm.ball().pos();
}

/*-------------------------------------------------------------------*/
/*!

 */
bool HFOTrainer::inHFOArea(const Vector2D& pos) {
  // return (pos.x >= 0 &&
  //         pos.x <= 0.5 * ServerParam::instance().DEFAULT_PITCH_LENGTH &&
  //         std::fabs(pos.y) <=
  //             0.5 * ServerParam::instance().DEFAULT_PITCH_WIDTH);
}

/*-------------------------------------------------------------------*/
/*!

 */
void HFOTrainer::logEpisode(const char* endCond) {
  std::cout << world().time().cycle() << "," << endCond << std::endl;
}

/*-------------------------------------------------------------------*/
/*!
    来自server源码的实现法，建议用check_ball重写
 */
bool HFOTrainer::crossGoalLine(const SideID side,
                               const Vector2D& prev_ball_pos) {
  const CoachWorldModel& wm = world();
  if (prev_ball_pos.x == wm.ball().pos().x) {
    // ball cannot have crossed gline
    //          std::cout << time << ": vertcal movement\n";
    return false;
  }
  if (std::fabs(wm.ball().pos().x) <=
      ServerParam::DEFAULT_PITCH_LENGTH * 0.5 + ServerParam::i().ballSize()) {
    // ball hasn't crossed gline
    //          std::cout << time << ": hasn't crossed\n";
    return false;
  }
  if (std::fabs(prev_ball_pos.x) >
      ServerParam::DEFAULT_PITCH_LENGTH * 0.5 + ServerParam::i().ballSize()) {
    // ball already over the gline
    //          std::cout << time << ": already crossed\n";
    return false;
  }
  if ((side * wm.ball().pos().x) >= 0) {
    // ball in wrong half
    //          std::cout << time << ": wrong_half\n";
    return false;
  }

  if (std::fabs(prev_ball_pos.y) > (ServerParam::instance().goalWidth() * 0.5 +
                                    ServerParam::instance().goalPostRadius()) &&
      std::fabs(prev_ball_pos.x) > ServerParam::DEFAULT_PITCH_LENGTH * 0.5) {
    // then the only goal that could have been scored would be
    // from going behind the goal post.  I'm pretty sure that
    // isn't possible anyway, but just in case this function acts
    // as a double check
    //          std::cout << time << ": behind_half\n";
    return false;
  }
  double delta_x = wm.ball().pos().x - prev_ball_pos.x;
  double delta_y = wm.ball().pos().y - prev_ball_pos.y;

  // we already checked above that ball->pos.x != prev_ball_pos.x, so delta_x
  // cannot be zero.
  double gradient = delta_y / delta_x;
  double offset = prev_ball_pos.y - gradient * prev_ball_pos.x;

  // determine y for x = ServerParam::PITCH_LENGTH*0.5 +
  // ServerParam::instance().ballSize() * -side
  double x = (ServerParam::DEFAULT_PITCH_LENGTH * 0.5 +
              ServerParam::instance().ballSize()) *
             -side;
  double y_intercept = gradient * x + offset;

  return std::fabs(y_intercept) <= (ServerParam::instance().goalWidth() * 0.5 +
                                    ServerParam::instance().goalPostRadius());
}

/*-------------------------------------------------------------------*/
/*!

 */
void HFOTrainer::resetField() {
  const CoachWorldModel& wm = world();

  //获取球可以被摆放的范围
  //使用种子随机生成球的位置
  //摆放球
  //   double pitch_length = ServerParam::DEFAULT_PITCH_LENGTH;
  //   double half_pitch_length = 0.5 * pitch_length;
  //   double pitch_width = ServerParam::DEFAULT_PITCH_WIDTH;
  //   double min_ball_x =
  //       std::max(std::min(M_hfo_param.M_hfo_min_ball_pos_x, 1.), 0.);
  //   double max_ball_x =
  //       std::max(std::min(M_hfo_param.M_hfo_max_ball_pos_x, 1.), 0.);
  //   max_ball_x = std::max(max_ball_x, min_ball_x);
  //   double ball_x = drand(min_ball_x * half_pitch_length,
  //                         max_ball_x * half_pitch_length, M_rng);
  //   double min_ball_y =
  //       std::max(std::min(M_hfo_param.M_hfo_min_ball_pos_y, 1.), -1.);
  //   double max_ball_y =
  //       std::max(std::min(M_hfo_param.M_hfo_max_ball_pos_y, 1.), -1.);
  //   max_ball_y = std::max(max_ball_y, min_ball_y);
  //   double ball_y = drand(min_ball_y / 2.0 * pitch_width,
  //                         max_ball_y / 2.0 * pitch_width, M_rng);
  // doMoveBall(Vector2D(ball_x, ball_y), Vector2D(0.0, 0.0));
  // 这里是随机速度，还没有写

  double ball_pos_x = M_hfo_param.M_hfo_max_ball_pos_x;
  double ball_pos_y = M_hfo_param.M_hfo_max_ball_pos_y;
  double ball_vel_x = M_hfo_param.M_hfo_max_ball_vel_x;
  double ball_vel_y = M_hfo_param.M_hfo_max_ball_vel_y;
  doMoveBall(Vector2D(ball_pos_x, ball_pos_y),
             Vector2D(ball_vel_x, ball_vel_y));

  //使用种子随机生成位置
  //开启直接将球给进攻球员时，随机球给到的球员
  M_prev_ball_pos = wm.ball().pos();
  boost::variate_generator<boost::mt19937&, boost::uniform_int<> > gen(
      M_rng, boost::uniform_int<>());
  std::random_shuffle(M_offsets.begin(), M_offsets.end(), gen);
  int offense_pos_on_ball = -1;
  int offense_count = 0;
  int hfo_offense_on_ball = M_hfo_param.M_hfo_offense_on_ball;
  const auto end = wm.allPlayers().end();
  if (hfo_offense_on_ball > 0) {
    for (auto p = wm.allPlayers().begin(); p != end; ++p) {
      if ((*p)->isValid() && (*p)->side() == LEFT) {
        offense_count++;
      }
    }
    if (hfo_offense_on_ball > offense_count) {
      offense_pos_on_ball = irand(offense_count);
    } else {
      offense_pos_on_ball = hfo_offense_on_ball - 1;
    }
  }

  //摆放双方球员
  //   int offense_pos = 0;
  //   for (auto p = wm.allPlayers().begin(); p != end; ++p) {
  //     if (!(*p)->isValid()) continue;
  //     double x, y;
  //     if ((*p)->side() == LEFT) {
  //       if (offense_pos_on_ball == offense_pos) {
  //         // 当开启直接将球给球员时的摆放方式
  //         doMovePlayer(world().teamNameLeft(), (*p)->unum(),
  //                      Vector2D(ball_x - .1, ball_y));
  //         offense_pos++;
  //         continue;
  //       }
  //       //使用随机便宜量
  //       std::pair<int, int> offset = M_offsets[offense_pos];
  //       x = ball_x + .1 * pitch_length * (drand(0, 1, M_rng) + offset.first);
  //       y = ball_y + .1 * pitch_length * (drand(0, 1, M_rng) +
  //       offset.second); x = std::min(std::max(x, -.1), half_pitch_length); y
  //       = std::min(std::max(y, -.4 * pitch_width), .4 * pitch_width);
  //       doMovePlayer(world().teamNameLeft(), (*p)->unum(), Vector2D(x, y));
  //       offense_pos++;
  //     } else if ((*p)->side() == RIGHT) {
  //       if ((*p)->goalie()) {
  //         x = .5 * pitch_length;
  //         y = 0;
  //       } else {
  //         //防守方球员的位置可调
  //         x = drand(.4 * pitch_length, .5 * pitch_length, M_rng);
  //         y = drand(-.4 * pitch_width, .4 * pitch_width, M_rng);
  //       }
  //       doMovePlayer(world().teamNameRight(), (*p)->unum(), Vector2D(x, y));
  //     }
  //   }
  double player_x = M_hfo_param.M_hfo_max_player_pos_x;
  double player_y = M_hfo_param.M_hfo_max_player_pos_y;
  doMovePlayer(world().teamNameLeft(), wm.allPlayers().at(0)->unum(),
               Vector2D(player_x, player_y));
  //回复球员状态
  //重置时间等球场状态
  doRecover();
  M_take_time = 0;
  M_untouched_time = 0;
  M_holder_side = 'U';
  M_holder_unum = -1;
  M_time = wm.time().cycle();
}
