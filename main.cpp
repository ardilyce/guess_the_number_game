//
//Created by Ardil Yuce
//

#include <iostream>
#include <chrono> //for timing
#include <ctime> //for time_t, tm, localtime, mktime
#include <iomanip> //for put_time
#include <thread>
#include <mutex>
#include <random>
#include <vector>
using namespace std;

mutex cout_mutex; //mutex to prevent printing at the same time
int target, rand_lower_bound, rand_upper_bound;
vector<int> wins;
int winner_id = -1;
bool start_signal = false;
bool game_over = false;

//Begin: Taken from "7-multithreading.pptx"
int random_range(const int & min, const int & max)
{
    static mt19937 generator(time(0));
    uniform_int_distribution<int> distribution(min,max);
    return distribution(generator);
}
//End: Taken from "7-multithreading.pptx"


void player_function(int player_id) //function executed by each player thread
{
    while(!start_signal) //wait until the start signal is given
    {
        this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    this_thread::sleep_until( chrono::system_clock::now()+chrono::seconds(3)); //wait 3 seconds in the first round

    while (!game_over)
    {
        //get the current time
        time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());
        struct tm *ptm = localtime(&tt);

        int guess = random_range(rand_lower_bound, rand_upper_bound); //make a guess

        cout_mutex.lock();
        if (guess == target)
        {
            if (winner_id == -1 and start_signal)
            {
                winner_id = player_id;
                cout << "Player with id " << player_id << " guessed " << guess << " correctly at: " << put_time(ptm, "%X") << endl << endl;
                cout_mutex.unlock();
                this_thread::sleep_for(chrono::seconds(1)); //wait one second before the next guess
            }
            else {cout_mutex.unlock();}

        }
        else
        {
            if (winner_id == -1 and start_signal)
            {
                cout << "Player with id " << player_id << " guessed " << guess << " incorrectly at: " << put_time(ptm,"%X") << endl;
                cout_mutex.unlock();
                this_thread::sleep_for(chrono::seconds(1)); //wait one second before the next guess
            }
            else {cout_mutex.unlock();}
        }

    }
}

void host_function(int number_of_players, int number_of_rounds) //function executed by host thread to manage the game
{
    wins.resize(number_of_players, 0); //initialize the wins vector

    //create and start player threads
    vector<thread> players;
    for (int i = 0; i < number_of_players; i++)
    {
        players.push_back(thread(player_function, i));
    }

    cout << endl;
    for (int i = 0; i < number_of_rounds; i++)
    {
        winner_id = -1; //reset winner_id for new round
        time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());
        struct tm *ptm = localtime(&tt);

        //start of the new round
        cout_mutex.lock();
        cout << "---------------------------------------------------" << endl;
        if (i == 0) //first round of the game
        {
            cout << "Game started at: " << put_time(ptm,"%X")<<endl;
            cout << "Round 1 will start 3 seconds later" << endl << endl;
        }
        else
        {
            cout << "Round " << i+1 << " started at: " << put_time(ptm,"%X") << endl;
        }

        target = random_range(rand_lower_bound, rand_upper_bound); //set a target for the round

        cout << "Target is " << target << endl << endl;
        cout_mutex.unlock();

        start_signal = true; //signal players to start

        while (winner_id == -1) //wait until a player wins the round
        {
            this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        start_signal = false;
        wins[winner_id]++; //update the wins count;
    }

    game_over = true; //notify all threads that game is over

    for (int i = 0; i < number_of_players; i++) //wait for all player threads to finish
    {
        players[i].join();
    }

    //print the final leaderboard
    cout_mutex.lock();
    cout << "Game is over!" << endl;
    cout <<"Leaderboard:" << endl;
    for (int i = 0; i < number_of_players; i++)
    {
        cout << "Player " << i << " has won " << wins[i] << " times" << endl;
    }
    cout_mutex.unlock();

}
int main()
{
    int number_of_players, number_of_rounds;
    do {
        cout << "Please enter number of players" << endl;
        cin >> number_of_players;
        if (number_of_players < 1)
        {
            cout << "Number of players cannot be lower than 1!" << endl;
        }
    }while(number_of_players < 1);

    do {
        cout << "Please enter number of rounds" << endl;
        cin >>number_of_rounds;
        if (number_of_rounds < 1)
        {
            cout << "Number of rounds cannot be lower than 1!" << endl;
        }
    }while(number_of_rounds < 1);

    do {
        cout << "Please enter the randomization range" << endl;
        cin >> rand_lower_bound >> rand_upper_bound;
        if (rand_lower_bound > rand_upper_bound)
        {
            cout << "Lower bound has to be smaller than or equal to higher bound" << endl;
        }
    }while(rand_lower_bound > rand_upper_bound);

    thread host_thread(host_function, number_of_players, number_of_rounds); //create and start the host thread
    host_thread.join();

    return 0;
}
