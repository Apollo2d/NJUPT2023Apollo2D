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

#ifndef HFO_TRAINER_H
#define HFO_TRAINER_H

#include <rcsc/geom/vector_2d.h>
#include <rcsc/trainer/trainer_agent.h>
#include <rcsc/types.h>

#include <boost/random.hpp>
#include <set>
#include <vector>

#include "hfo_param.h"
using namespace rcsc;

class HFOTrainer : public rcsc::TrainerAgent {
 private:
 public:
  HFOTrainer();

  virtual ~HFOTrainer();

 protected:
  /*!
    You can override this method.
    But you must call TrainerAgent::doInit() in this method.
  */
  virtual bool initImpl(rcsc::CmdLineParser& cmd_parser);

  //! main decision
  virtual void actionImpl();

  virtual void handleInitMessage();
  virtual void handleServerParam();
  virtual void handlePlayerParam();
  virtual void handlePlayerType();

 private:
  void sampleAction();
  void recoverForever();
  void doSubstitute();
  void doKeepaway();

  // HFO
 public:
  HFOParam M_hfo_param;
  void doHFO();
  void initHFO();
  void analyse();

 private:
  bool inHFOArea(const Vector2D& pos);
  // void logHeader();
  void logEpisode(const char* endCond);
  bool crossGoalLine(const SideID side, const Vector2D& prev_ball_pos);
  void resetField();

 private:
  int M_episode;
  int M_offense, M_defense;
  int M_time;
  int M_take_time;
  int M_holder_unum;
  char M_holder_side;  // 'L' = left, 'R' = Right, 'U' = Unknown/Neutral
  Vector2D M_prev_ball_pos;
  int M_untouched_time;
  int M_episode_begin_time;
  int M_episode_over_time;
  boost::mt19937 M_rng;
  std::vector<std::pair<int, int> > M_offsets;
};

#endif
