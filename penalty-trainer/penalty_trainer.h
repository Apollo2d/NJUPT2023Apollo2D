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

#ifndef PENALTY_TRAINER_H
#define PENALTY_TRAINER_H

#include <rcsc/trainer/trainer_agent.h>

class PenaltyTrainer : public rcsc::TrainerAgent
{
public:
  PenaltyTrainer();

  virtual ~PenaltyTrainer();

protected:
  /*!
    You can override this method.
    But you must call TrainerAgent::doInit() in this method.
  */
  virtual bool initImpl(rcsc::CmdLineParser &cmd_parser);

  //! main decision
  virtual void actionImpl();

  virtual void handleInitMessage();
  virtual void handleServerParam();
  virtual void handlePlayerParam();
  virtual void handlePlayerType();

public:
  static unsigned ROUND_NUM;
  static unsigned MAX_ROUND;
private:
  rcsc::Vector2D prev_ball_pos;
  enum
  {
    SCORE = 0,
    CAUGHT,
    MISS
  } result;
  enum
  {
    BEFORE = 0,
    SETUP,
    READY,
    TAKEN
  } status = BEFORE;
  long timer;

private:
  void initPenalty();
  void doPenalty();
  bool pend();
  void analyse();
  void finalise();
  void print();
  void stat();
  bool ballCaught(const rcsc::CoachPlayerObject* goalie);
  bool crossGoalLine(const rcsc::SideID side, const rcsc::Vector2D &prev_ball_pos);
};

#endif
