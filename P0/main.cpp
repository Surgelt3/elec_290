#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <math.h>

typedef const char *String;

/*
  Enumerated type for the round results.
*/
enum RoundResult
{
  Round_None = 0,
  Round_Lost = 1,
  Round_Draw = 2,
  Round_Won = 3
};

/*
  Enumerated type for the rock paper scissors choices
*/
enum RPSChoice
{
  RPS_None = -1,
  RPS_Rock = 0,
  RPS_Paper = 1,
  RPS_Scissors = 2
};

/*
  Global variables to manage the the rounds
  MAX_ROUNDS : Maximum number of rounds Arduino can play before it has to be reset, in essence how many rounds it can remember
  roundsPlayed : The number of rounds played by the Arduino
  arduinoChoices : The set of RPS Choices made by the Arduino in previous rounds.
  opponentChoices : The set of RPS Choices made by the opponent in previous rounds.
  roundResults : The set of round results for previous rounds.
*/
const int MAX_ROUNDS = 500;
int roundsPlayed;
RPSChoice arduinoChoices[MAX_ROUNDS];
RPSChoice opponentChoices[MAX_ROUNDS];
RoundResult roundResults[MAX_ROUNDS];

RoundResult determineRoundResults(RPSChoice arduinoChoice, RPSChoice opponentChoice)
{
  /*
    Returns enum indicating whether arduino won, lost or was a draw.
  */

  // should never have RPS_None here but handle it just in case
  if (arduinoChoice == RPS_None || opponentChoice == RPS_None)
  {
    return Round_None;
  }

  /*
    We can use simple math to solve the problem
    Since rock < paper < scissors < rock ...,
    Is cyclic, we can use modulo, hence
    rock = 0
    paper = 1
    scissors = 2

    For a given choice at i, choice at (i+1 % 3) always beats choice at i
  */

  if (opponentChoice == (arduinoChoice + 1) % 3)
  {
    // opponent selected choice that beats Arduino
    return Round_Lost;
  }
  else if (opponentChoice == arduinoChoice)
  {
    return Round_Draw;
  }
  else
  {
    return Round_Won;
  }
}

void saveRound(RPSChoice arduinoChoice, RPSChoice opponentChoice, RoundResult roundResult)
{
  /*
    Saves the results of the round in the arrays and increments our roundCount
    arduinoChoice is saved in arduinoChoices array
    opponentChoice is saved in opponentChoices array
    roundResult is saved in roundResults array
  */

  arduinoChoices[roundsPlayed] = arduinoChoice;
  opponentChoices[roundsPlayed] = opponentChoice;
  roundResults[roundsPlayed] = roundResult;
  roundsPlayed++;
}

String RoundResToWinner(RoundResult gs)
{
  /*
    Returns the string of the round winner based on the round result
  */
  switch (gs)
  {
  case Round_Draw:
    return "Draw";

  case Round_Won:
    return "Arduino";

  case Round_Lost:
    return "Opponent";

  default:
    return "None";
  }
}

String RPSChoiceToStr(RPSChoice choice)
{
  /*
    Returns the string version of an RPS choice
  */
  switch (choice)
  {
  case RPS_Rock:
    return "Rock";

  case RPS_Paper:
    return "Paper";

  case RPS_Scissors:
    return "Scissors";

  default:
    return "None";
  }
}

RPSChoice randRPS()
{
  /*
    Returns a random rps choice
  */
  return (RPSChoice)(rand() % 3);
}

RPSChoice winningChoiceAgainst(RPSChoice choice)
{
  /*
    Returns the RPS choice that wins against choice.
    e.g. winningChoiceAgainst(RPS_Rock) = RPS_Paper
  */
  return (RPSChoice)((choice + 1) % 3);
}

RPSChoice losingChoiceAgainst(RPSChoice choice)
{
  /*
    Returns the RPS choice that loses against choice.
    e.g. losingChoiceAgainst(RPS_Rock) = RPS_Scissors
  */
  return (RPSChoice)((choice - 1 + 3) % 3);
}

//******************************************************************
// TODO Modify this function
//******************************************************************

namespace lucas
{
  //******************************************************************
  // TODO Modify this function
  //******************************************************************

  enum strategies
  {
    Random_Choice = 0,
    Beat_Last_Input = 1,
    Play_Previous_Losing = 2,
    Counter_Rotation = 3,
    Win_Stay_Lose_Shift = 4,
    Win_Replay_Lose_Shift = 5
  };

  RPSChoice beat_last_input(int prev_round)
  {
    return (RPSChoice)((opponentChoices[prev_round] + 1) % 3);
  }

  RPSChoice beat_last_input_counter(int prev_round)
  {
    return (RPSChoice)winningChoiceAgainst((RPSChoice)prev_round);
  }

  // unsure if this is correct
  RPSChoice play_prev_losing(int prev_round)
  {
    RPSChoice choice = arduinoChoices[prev_round];
    RPSChoice losing_choice = losingChoiceAgainst(choice);
    return losing_choice;
  }

  RPSChoice play_prev_losing_counter(int prev_round)
  {

    RPSChoice normal_choice = play_prev_losing(prev_round);
    RPSChoice counter_choice = winningChoiceAgainst(normal_choice);
    return counter_choice;
  }

  RPSChoice counter_opp_rotation(int prev_round)
  {
    RPSChoice prev_round_results = opponentChoices[prev_round];
    RPSChoice prev_round_results_2 = opponentChoices[prev_round - 1];

    RPSChoice rotation = (RPSChoice)(prev_round_results_2 - prev_round_results);

    RPSChoice rotation_counter = (RPSChoice)(opponentChoices[prev_round] + rotation + 1);
    if (rotation_counter == 3)
    {
      rotation = (RPSChoice)0;
    }

    return rotation_counter;
  }

  RPSChoice counter_opp_rotation_counter(int prev_round)
  {

    RPSChoice normal_choice = counter_opp_rotation(prev_round);
    RPSChoice counter_choice = winningChoiceAgainst(normal_choice);
    return counter_choice;
  }

  RPSChoice win_stay_lose_shift(int prev_round)
  {

    RPSChoice choice;
    if (roundResults[prev_round] == Round_Won)
    {
      choice = opponentChoices[prev_round];
    }
    else
    {
      choice = winningChoiceAgainst(opponentChoices[prev_round]);
    }
    return choice;
  }

  RPSChoice win_stay_lose_shift_counter(int prev_round)
  {
    RPSChoice choice;
    RPSChoice opp_move = win_stay_lose_shift(prev_round);
    choice = winningChoiceAgainst(opp_move);
    return choice;
  }

  RPSChoice win_replay_lose_shift(int prev_round)
  {
    RPSChoice choice;
    if (roundResults[prev_round] == Round_Won)
    {
      choice = arduinoChoices[prev_round];
    }
    else
    {
      choice = winningChoiceAgainst(opponentChoices[prev_round]);
    }
    return choice;
  }

  RPSChoice win_replay_lose_shift_counter(int prev_round)
  {
    RPSChoice choice;
    RPSChoice opp_move = win_replay_lose_shift(prev_round);
    choice = winningChoiceAgainst(opp_move);
    return choice;
  }

  //******************************************************************
  // TODO Modify this function
  //******************************************************************

  RPSChoice use_strategy(strategies strategy)
  {
    switch (strategy)
    {
    case (0):
      return randRPS();
    case (1):
      return counter_opp_rotation(roundsPlayed - 1);
    case (2) :
      return beat_last_input_counter(roundsPlayed - 1);
    case (3):
      return play_prev_losing_counter(roundsPlayed - 1);
    case (4):
      return counter_opp_rotation_counter(roundsPlayed - 1);
    case (5):
      return win_stay_lose_shift_counter(roundsPlayed - 1);
    case (6):
      return win_replay_lose_shift_counter(roundsPlayed - 1);
    }
  }

  strategies guess_strategy()
  {

    int counter = 0;
    strategies list_strats[6];

    if (win_replay_lose_shift(roundsPlayed - 2) == opponentChoices[roundsPlayed - 1])
    {
      list_strats[counter] = Win_Replay_Lose_Shift;
      counter++;
    }
    else if (win_stay_lose_shift(roundsPlayed - 2) == opponentChoices[roundsPlayed - 1])
    {
      list_strats[counter] = Win_Stay_Lose_Shift;
      counter++;
    }
    else if (counter_opp_rotation(roundsPlayed - 2) == opponentChoices[roundsPlayed - 1])
    {
      list_strats[counter] = Counter_Rotation;
      counter++;
    }
    else if (play_prev_losing(roundsPlayed - 2) == opponentChoices[roundsPlayed - 1])
    {
      list_strats[counter] = Beat_Last_Input;
      counter++;
    }
    else if (beat_last_input(roundsPlayed - 2) == opponentChoices[roundsPlayed - 1])
    {
      list_strats[counter] = Play_Previous_Losing;
      counter++;
    }
    else
    {
      list_strats[counter] = Random_Choice;
      counter++;
      list_strats[counter] = Counter_Rotation;
      counter++;
    }

    strategies choice = list_strats[rand()%counter];
    return choice;
  }

  RPSChoice makeRPSChoice()
  {
    /*
      Returns choice made by an Arduino for a round.

      TODO: Complete this function.

      Available global variables:
        arduinoChoices : The set of RPS Choices made by the Arduino in previous rounds.
        opponentChoices : The set of RPS Choices made by the opponent in previous rounds.
        roundResults : The set of round results for previous rounds.
        roundsPlayed : The number of rounds played by the Arduino.

      Available functions:
        randRPS
        winningChoiceAgainst
        losingChoiceAgainst
        RPSChoiceToStr
        RoundResToWinner
    */

    strategies strategy;
    if (roundsPlayed == 0)
    {
      strategy = Random_Choice;
    }
    else if (roundsPlayed == 1)
    {
      strategy = (strategies)(rand()%7);
    }
    else if (roundsPlayed > 1)
    {
      strategy = guess_strategy();
    }

    // fixed choice implementation
    RPSChoice choice;
    choice = use_strategy(strategy);
    return choice;
  }
}
RPSChoice beatLast()
{
  if (!roundsPlayed || ((roundsPlayed - 1) % 3 == 0 && roundsPlayed != 1))
    // if(!roundsPlayed)
    return randRPS();
  return winningChoiceAgainst(opponentChoices[roundsPlayed - 1]);
}

RPSChoice custom()
{
  RPSChoice choice;
  if (roundsPlayed == 0 || roundsPlayed == 3 || roundsPlayed == 6)
  {
    choice = randRPS();
    return choice;
  }
  else
  {
    choice = winningChoiceAgainst(opponentChoices[roundsPlayed - 1]);
    return choice;
  }
}

RPSChoice counterRotation()
{
  if (roundsPlayed == 0)
    return randRPS();
  else if (roundsPlayed == 1)
    return losingChoiceAgainst(arduinoChoices[0]);
  int rot = (opponentChoices[roundsPlayed - 1] - opponentChoices[roundsPlayed - 2] + 3) % 3;
  return winningChoiceAgainst((RPSChoice)((opponentChoices[roundsPlayed - 1] + rot) % 3));
}

RPSChoice counterRotationAcc()
{
  if (roundsPlayed == 0)
    return randRPS();
  else if (roundsPlayed == 1)
    return losingChoiceAgainst(arduinoChoices[0]);
  int rot[4] = {0, 0, 0, 0};
  int n = 0;
  for (int i = 0; i < 4 && roundsPlayed - 2 - i >= 0; i++)
  {
    rot[i] = (opponentChoices[roundsPlayed - 1 - i] - opponentChoices[roundsPlayed - 2 - i] + 3) % 3;
    n++;
  }
  float acc = 0;
  for (int i = 0; i < n - 1; i++)
  {
    acc += rot[0 + i] - rot[1 + i];
  }
  acc /= n;
  return winningChoiceAgainst((RPSChoice)((opponentChoices[roundsPlayed - 1] + rot[0] + (int)acc) % 3));
}

RPSChoice makeRPSChoice()
{
  /*
    Returns choice made by an Arduino for a round.

    TODO: Complete this function.

    Available global variables:
      arduinoChoices : The set of RPS Choices made by the Arduino in previous rounds.
      opponentChoices : The set of RPS Choices made by the opponent in previous rounds.
      roundResults : The set of round results for previous rounds.
      roundsPlayed : The number of rounds played by the Arduino.

    Available functions:
      randRPS
      winningChoiceAgainst
      losingChoiceAgainst
      RPSChoiceToStr
      RoundResToWinner
  */

  // fixed choice implementation
  RPSChoice choice;
  choice = counterRotationAcc();
  // choice=RPS_Paper;
  return choice;
}
//******************************************************************
//******************************************************************
void loop();
int main()
{
  // Set the frequency of the serial output, will be the frequency required in the serial monitor
  for (int i = 0; i < MAX_ROUNDS; i++)
  {
    arduinoChoices[i] = RPS_None;
    opponentChoices[i] = RPS_None;
    roundResults[i] = Round_None;
  }

  // Initialize seed for random,
  srand(time(NULL));
  while (1)
  {
    loop();
  }
}

void println(String a)
{
  printf("%s\n", a);
}

void loop()
{

  if (roundsPlayed >= MAX_ROUNDS)
  {
    int win = 0, lost = 0, tie = 0;
    for (int i = 0; i < MAX_ROUNDS; i++)
    {
      if (roundResults[i] == Round_Lost)
        lost++;
      else if (roundResults[i] == Round_Won)
        win++;
      else if (roundResults[i] == Round_Draw)
        tie++;
    }
    printf("won: %d\nlost: %d\ntie: %d\n", win, lost, tie);
    exit(0);
    return;
  }

  // Wait for the turn button to be pressed
  println("Waiting for round start...");
  println("Started Round: ");
  printf("%d\n", roundsPlayed + 1);

  RPSChoice opponentChoice, arduinoChoice;
  RoundResult result;

  // Arduino makes its choice first
  arduinoChoice = makeRPSChoice();

  // show arduino choice
  println("Arduino selected: ");
  println(RPSChoiceToStr(arduinoChoice));

  // Now listen to get the choice the opponent made
  // While listening  for opponent choice, we show the arduino choice
  println("Waiting for opponent choice...");
  opponentChoice = lucas::makeRPSChoice();

  println("Opponent selected: ");
  println(RPSChoiceToStr(opponentChoice));

  // determine the result
  result = determineRoundResults(arduinoChoice, opponentChoice);

  // save the result
  // Serial.println("Saving round information.");
  saveRound(arduinoChoice, opponentChoice, result);

  // Show round results on LEDs while listening for end round
  println("Round Winner: ");
  println(RoundResToWinner(result));

  println("=======================>");
}