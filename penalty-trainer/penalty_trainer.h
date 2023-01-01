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

class PenaltyTrainer : public rcsc::TrainerAgent {
   private:
   public:
    PenaltyTrainer();

    virtual ~PenaltyTrainer();

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
    void recoverForever();
    void doSubstitute();
    void doKeepaway();
  
   private:
    const unsigned M_MAX_ROUND = 30;
    
    bool before_round;
    unsigned round;
    enum {
      SCORE = 0, CAUGHT, MISS
    } result;
    unsigned score = 0, miss = 0;
    bool keeper_dir, ball_dir;
   private:
    void initPenalty();
    void doPenalty();
    void analyse();
    void finalise();
    void print();
    void conclude();
};

#endif
