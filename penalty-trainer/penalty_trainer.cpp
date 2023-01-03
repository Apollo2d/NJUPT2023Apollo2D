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

#include "penalty_trainer.h"

#include <iomanip>

using namespace rcsc;

/*-------------------------------------------------------------------*/
/*!

 */
PenaltyTrainer::PenaltyTrainer()
    : TrainerAgent()
{
}

/*-------------------------------------------------------------------*/
/*!

 */
PenaltyTrainer::~PenaltyTrainer() {}

/*-------------------------------------------------------------------*/
/*!

 */
bool PenaltyTrainer::initImpl(CmdLineParser &cmd_parser)
{
    bool result = TrainerAgent::initImpl(cmd_parser);

    if(!result) { return false; }

    ParamMap my_params;

    my_params.add()("num", "", &round);
    my_params.add()("total", "", &MAX_ROUND);

    cmd_parser.parse( my_params );

    if(cmd_parser.failed()) {
        std::cerr << "coach: ***WARNING*** detected unsupported options: ";
        cmd_parser.print(std::cerr);
        std::cerr << std::endl;
    }
    //////////////////////////////////////////////////////////////////
    // Add your code here.
    if (round > MAX_ROUND)
        return false;
    //////////////////////////////////////////////////////////////////

    return true;
}

/*-------------------------------------------------------------------*/
/*!

 */
void PenaltyTrainer::actionImpl()
{
    if (world().teamNameLeft().empty() || world().teamNameRight().empty())
        return;

    doPenalty();
}

/*-------------------------------------------------------------------*/
/*!

*/
void PenaltyTrainer::handleInitMessage() {}

/*-------------------------------------------------------------------*/
/*!

*/
void PenaltyTrainer::handleServerParam() {}

/*-------------------------------------------------------------------*/
/*!

*/
void PenaltyTrainer::handlePlayerParam() {}

/*-------------------------------------------------------------------*/
/*!

*/
void PenaltyTrainer::handlePlayerType() {}

void PenaltyTrainer::doPenalty()
{       
    if (!status)
        initPenalty();
    if (pend()) {
        analyse();
        prev_ball_pos = world().ball().pos();
    }
}

void PenaltyTrainer::initPenalty() { 
    doMoveBall(Vector2D(0, 0), Vector2D(0, 0));
    timer = world().time().cycle();
    doChangeMode(PM_PenaltySetup_Left);
    status = SETUP;
}

bool PenaltyTrainer::pend() {
    switch (status) {        
        case SETUP: 
            if (world().time().cycle() > timer + ServerParam::DEFAULT_PEN_SETUP_WAIT) {
                doChangeMode(PM_PenaltyReady_Left);
                timer = world().time().cycle();
                status = READY;
            }break;
        case READY:
            if (world().time().cycle() > timer + ServerParam::DEFAULT_PEN_READY_WAIT) {    
                doChangeMode(PM_PenaltyTaken_Left);
                status = TAKEN;  
                std::cout << "ROUND " << round << std::endl;
                std::cout << world().teamNameLeft() << " vs. " << world().teamNameRight() << std::endl;
            }break;
        case TAKEN: return true;
    }
    return false;
}

bool PenaltyTrainer::crossGoalLine(const SideID side, const Vector2D& prev_ball_pos) {
    const CoachWorldModel& wm = world();
    if(prev_ball_pos.x == wm.ball().pos().x) {
        // ball cannot have crossed gline
        //          std::cout << time << ": vertcal movement\n";
        return false;
    }
    if(std::fabs(wm.ball().pos().x) <=
       ServerParam::DEFAULT_PITCH_LENGTH * 0.5 + ServerParam::i().ballSize()) {
        // ball hasn't crossed gline
        //          std::cout << time << ": hasn't crossed\n";
        return false;
    }
    if((side * wm.ball().pos().x) >= 0) {
        // ball in wrong half
        //std::cout << wm.time() << ": wrong_half\n";
        return false;
    }
    return true;
}

void PenaltyTrainer::analyse() {
    /*for (auto i : world().allPlayers())
        if ( i->goalie() && ) {
            doChangeMode(PM_PenaltyMiss_Left);
            result = CAUGHT;
            finalise();
            return;
    }*/
    if ( crossGoalLine( SideID::RIGHT, prev_ball_pos ) ) {
        doChangeMode(PM_PenaltyScore_Left);
        result = SCORE;
        finalise();
        return;
    }
    if ( std::fabs( world().ball().pos().x )
                  > ServerParam::DEFAULT_PITCH_LENGTH * 0.5
                  + ServerParam::instance().ballSize()
                  || std::fabs( world().ball().pos().y )
                  > ServerParam::DEFAULT_PITCH_WIDTH * 0.5
                  + ServerParam::instance().ballSize() )
    {
        doChangeMode(PM_PenaltyMiss_Left);
        result = MISS;
        finalise();
        return;
    }
}

void PenaltyTrainer::finalise() {
    if (result == SCORE)
        score++;
    else if (result == MISS)
        miss++;
    print();
    finalize();
    exit(0);
}

void PenaltyTrainer::print() {
    switch (result) {
        case MISS:std::cout << world().teamNameRight() << " missed!" << std::endl;break;
        case SCORE:std::cout << world().teamNameRight() << " scored!" << std::endl;break;
        case CAUGHT:std::cout << world().teamNameLeft() << " caught it!" << std::endl;break;
    }
}