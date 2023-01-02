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
#include "object.h"

#include <iomanip>

using namespace rcsc;

/*-------------------------------------------------------------------*/
/*!

 */
PenaltyTrainer::PenaltyTrainer()
    : TrainerAgent(),
      before_round(true)
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

    /* Example
        ParamMap my_params;

        std::string author;
        my_params.add()
            ("Author", "", &author, "Daedale");
            //( Key, ShortKey(Ommitable), &Value, Description )

        cmd_parser.parse( my_params );
    */

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
void PenaltyTrainer::actionImpl()
{
    if (world().teamNameLeft().empty() || world().teamNameRight().empty())
        return;

    //////////////////////////////////////////////////////////////////
    // Add your code here.

    // sampleAction();
    // recoverForever();
    // doSubstitute();
    // doKeepaway();
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
    if (before_round)
        initPenalty();
    if (round % 5 == 0)
        doRecover();
    if (pend())
        analyse();
    prev_ball_pos = world().ball().pos();
}

void PenaltyTrainer::initPenalty() { 
    before_round = false; 
    round++;
    status = BEFORE;
    timer = world().time().cycle();
    doChangeMode(PM_PenaltySetup_Left);
}

bool PenaltyTrainer::pend() {
    switch (status) {
        case TAKEN: return true;
        case BEFORE: 
            if (world().time().cycle() > timer + ServerParam::DEFAULT_PEN_SETUP_WAIT) {
                doChangeMode(PM_PenaltyReady_Left);
                timer = world().time().cycle();
                status = SETUP;
            }break;
        case SETUP:
            if (world().time().cycle() > timer + ServerParam::DEFAULT_PEN_READY_WAIT) {    
                doChangeMode(PM_PenaltyTaken_Left);
                status = READY;
            }break;
        case READY:   
                std::cout << "ROUND " << round << std::endl;
                std::cout << world().teamNameLeft() << " vs. " << world().teamNameRight() << std::endl;
                status = TAKEN;
                break;
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
    if(std::fabs(prev_ball_pos.x) >
       ServerParam::DEFAULT_PITCH_LENGTH * 0.5 + ServerParam::i().ballSize()) {
        // ball already over the gline
        //          std::cout << time << ": already crossed\n";
        return false;
    }
    if((side * wm.ball().pos().x) >= 0) {
        // ball in wrong half
        //          std::cout << time << ": wrong_half\n";
        return false;
    }

    if(std::fabs(prev_ball_pos.y) >
           (ServerParam::instance().goalWidth() * 0.5 +
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
    double offset   = prev_ball_pos.y - gradient * prev_ball_pos.x;

    // determine y for x = ServerParam::PITCH_LENGTH*0.5 +
    // ServerParam::instance().ballSize() * -side
    double x = (ServerParam::DEFAULT_PITCH_LENGTH * 0.5 +
                ServerParam::instance().ballSize()) *
               -side;
    double y_intercept = gradient * x + offset;

    return std::fabs(y_intercept) <=
           (ServerParam::instance().goalWidth() * 0.5 +
            ServerParam::instance().goalPostRadius());
}

bool PenaltyTrainer::caughtBall() {
    const ServerParam & SP = ServerParam::instance();

    auto goalie = world().playersRight().front();

    const double this_catch_area_delta = SP.catchAreaLength() * ( goalie->playerTypePtr()->catchAreaLengthStretch() - 1.0 );
    const double this_catch_area_l_max = SP.catchAreaLength() + this_catch_area_delta;
    const double this_catch_area_l_min = SP.catchAreaLength() - this_catch_area_delta;

    //const RArea default_catchable( PVector( SP.catchAreaLength()*0.5, 0.0 ),
    //                               PVector( SP.catchAreaLength(), SP.catchAreaWidth() ) );
    const RArea max_catchable( PVector( this_catch_area_l_max*0.5, 0.0 ),
                               PVector( this_catch_area_l_max, SP.catchAreaWidth() ) );
    const RArea min_catchable( PVector( this_catch_area_l_min*0.5, 0.0 ),
                               PVector( this_catch_area_l_min, SP.catchAreaWidth() ) );
    const double catch_angle = goalie->body().degree();

    auto delta = world().ball().pos() - goalie->pos();
    PVector rotated_pos(delta.x, delta.y);
    rotated_pos.rotate( -catch_angle );

    if ( ! max_catchable.inArea( rotated_pos ) )
        return false;

    bool success = false;
    static std::mt19937 engine;
    if ( min_catchable.inArea( rotated_pos ) )
    {
        //success = ( drand( 0, 1 ) <= SP.catchProb() );
        std::bernoulli_distribution dst( SP.catchProbability() );
        success = dst( engine );
        //std::cerr << M_stadium.time()
        //          << ": goalieCatch min_catchable ok" << std::endl;
    }
    else
    {
        double catch_prob
            = SP.catchProbability()
            - SP.catchProbability() * ( ( rotated_pos.x - this_catch_area_l_min )
                                        / ( this_catch_area_l_max - this_catch_area_l_min ) );
        catch_prob = std::min( std::max( 0.0, catch_prob ), 1.0 );

        //success = ( drand( 0, 1 ) <= catch_prob );
        std::bernoulli_distribution dst( catch_prob );
        success = dst( engine );
        //std::cerr << M_stadium.time()
        //          << ": goalieCatch "
        //          << " dir=" << Rad2Deg( normalize_angle( angleBodyCommitted() + NormalizeMoment( dir ) ) )
        //          << " x=" << rotated_pos.x
        //          << " min_l=" << this_catch_area_l_min
        //          << " max_l=" << this_catch_area_l_max
        //          << " catch_prob=" << catch_prob << std::endl;
    }
    return success;
}

void PenaltyTrainer::analyse() {
    if ( caughtBall() ) {
        doChangeMode(PM_PenaltyMiss_Left);
        result = CAUGHT;
        finalise();
        return;
    }
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
    before_round = true;
    if (round == M_MAX_ROUND) {
        conclude();
        finalize();
    }
}

void PenaltyTrainer::print() {
    switch (result) {
        case MISS:std::cout << world().teamNameRight() << " missed!" << std::endl;break;
        case SCORE:std::cout << world().teamNameRight() << " scored!" << std::endl;break;
        case CAUGHT:std::cout << world().teamNameLeft() << " caught it!" << std::endl;break;
    }
}

void PenaltyTrainer::conclude() {
    std::cout << "Training is over." << std::endl;
    std::cout << world().teamNameLeft() << " saved " << M_MAX_ROUND - score - miss << '.' << std::endl;
    std::cout << "Save Ratio is " << std::fixed << std::setprecision(1) << (M_MAX_ROUND - score - miss) * 100.0 / (M_MAX_ROUND - miss) << "%." << std::endl;
    std::cout << world().teamNameRight() << " scored " << score << '.' << std::endl;
    std::cout << "Score Ratio is " << std::fixed << std::setprecision(1) << score * 100.0 / M_MAX_ROUND << "%." << std::endl;
}