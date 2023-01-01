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
    {
        doTeamNames();
        return;
    }

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

/*-------------------------------------------------------------------*/
/*!

 */
void PenaltyTrainer::recoverForever()
{
    if (world().playersLeft().empty())
    {
        return;
    }

    if (world().time().stopped() == 0 && world().time().cycle() % 50 == 0)
    {
        // recover stamina
        doRecover();
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void PenaltyTrainer::doSubstitute()
{
    static bool s_substitute = false;
    if (!s_substitute && world().time().cycle() == 0 &&
        world().time().stopped() >= 10)
    {
        std::cerr << "trainer " << world().time()
                  << " team name = " << world().teamNameLeft() << std::endl;

        if (!world().teamNameLeft().empty())
        {
            UniformInt uni(0, PlayerParam::i().ptMax());
            doChangePlayerType(world().teamNameLeft(), 1, uni());

            s_substitute = true;
        }
    }

    if (world().time().stopped() == 0 && world().time().cycle() % 100 == 1 &&
        !world().teamNameLeft().empty())
    {
        static int type = 0;
        doChangePlayerType(world().teamNameLeft(), 1, type);
        type = (type + 1) % PlayerParam::i().playerTypes();
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void PenaltyTrainer::doKeepaway()
{
    if (world().trainingTime() == world().time())
    {
        std::cerr << "trainer: " << world().time() << " keepaway training time."
                  << std::endl;
    }
}

void PenaltyTrainer::doPenalty()
{   
    if (!world().gameMode().isPenaltyKickMode())
        return;
    if (round % 5 == 0)
        doRecover();
    if (before_round)
        initPenalty();
    analyse();
}

void PenaltyTrainer::initPenalty() {    
    before_round = false; 
    round++;   
    std::cout << "ROUND " << round << std::endl;
    std::cout << world().teamNameLeft() << " vs. " << world().teamNameRight() << std::endl;
    doChangeMode(PM_PenaltySetup_Left);
    doChangeMode(PM_PenaltyReady_Left);
    doMoveBall(rcsc::Vector2D(-rcsc::ServerParam::DEFAULT_PEN_DIST_X, 0), rcsc::Vector2D(0, 0));
    doMovePlayer(world().teamNameLeft(), 1, rcsc::Vector2D(-50, 0));
    doMovePlayer(world().teamNameRight(), 1, rcsc::Vector2D(rcsc::ServerParam::DEFAULT_PENALTY_SPOT_DIST - 50, 0));
}

void PenaltyTrainer::analyse() {
    switch (world().gameMode().type()) {
        case rcsc::GameMode::PenaltyMiss_:result = MISS;break;
        case rcsc::GameMode::PenaltyScore_:result = SCORE;break;
        case rcsc::GameMode::PenaltyTaken_:result = CAUGHT;break;
        default:return;
    }
    finalise();
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
        case MISS:std::cout << world().teamNameRight() << " missed!" << std::endl;
        case SCORE:std::cout << world().teamNameRight() << " scored!" << std::endl;
        case CAUGHT:std::cout << world().teamNameLeft() << " caught it!" << std::endl;
    }
}

void PenaltyTrainer::conclude() {
    std::cout << "Training is over." << std::endl;
    std::cout << world().teamNameLeft() << " saved " << M_MAX_ROUND - score - miss << '.' << std::endl;
    std::cout << "Save Ratio is " << std::fixed << std::setprecision(1) << M_MAX_ROUND - score - miss * 100.0 / (M_MAX_ROUND - miss ? M_MAX_ROUND - miss : 1) << "%." << std::endl;
    std::cout << world().teamNameRight() << " scored " << score << '.' << std::endl;
    std::cout << "Score Ratio is " << std::fixed << std::setprecision(1) << score * 100.0 / M_MAX_ROUND << "%." << std::endl;
    std::cout << "Miss Ratio is " << std::fixed << std::setprecision(1) << miss * 100.0 / M_MAX_ROUND << "%." << std::endl;
}