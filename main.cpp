//
//Created by Ardil Yuce
// 32375
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

mutex cout_mutex;
int target, rand_lower_bound, rand_upper_bound;
vector<int> wins;
int winner_id = -1;
bool start_signal = false;

//Begin: Taken from "7-multithreading.pptx"
int random_range(const int & min, const int & max)
{
    static mt19937 generator(time(0));
    uniform_int_distribution<int> distribution(min,max);
    return distribution(generator);
}
//End: Taken from "7-multithreading.pptx"

void player_function(int player_id)
{
    while(!start_signal)
    {
        this_thread::yield();
    }

    this_thread::sleep_until( chrono::system_clock::now()+chrono::seconds(3));

    while (winner_id == -1)
    {
        time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());
        tm ptm;
        localtime_r(&tt, &ptm);
        int guess = random_range(rand_lower_bound, rand_upper_bound);
        if (guess == target)
        {
            cout_mutex.lock();
            if (winner_id == -1)
            {
                winner_id = player_id;
                cout << "Player with id " << player_id << " guessed " << guess << " correctly at: " << put_time(&ptm, "%X") << endl << endl;
            }
            cout_mutex.unlock();
        }
        else
        {
            cout_mutex.lock();
            if (winner_id == -1)
            {
                cout << "Player with id " << player_id << " guessed " << guess << " incorrectly at: " << put_time(&ptm,"%X") << endl;
            }
            cout_mutex.unlock();
        }
        this_thread::sleep_for(chrono::seconds(1));
    }
}

void host_function(int number_of_players, int number_of_rounds)
{
    wins.resize(number_of_players, 0);

    vector<thread> players;
    for (int i = 0; i < number_of_players; i++)
    {
        players.push_back(thread(player_function, i));
    }

    cout << endl;
    for (int i = 0; i < number_of_rounds; i++)
    {
        winner_id = -1;
        time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());
        tm ptm;
        localtime_r(&tt, &ptm);

        cout_mutex.lock();
        cout << "---------------------------------------------------" << endl;
        cout_mutex.unlock();

        if (i == 0)
        {
            cout << "Game started at: " << put_time(&ptm,"%X")<<endl;
            cout << "Round 1 will start 3 seconds later" << endl << endl;
        }
        else
        {
            cout_mutex.lock();
            cout << "Round " << i+1 << " started at: " << put_time(&ptm,"%X") << endl;
            cout_mutex.unlock();
        }

        target = random_range(rand_lower_bound, rand_upper_bound);

        cout_mutex.lock();
        cout << "Target is " << target << endl << endl;
        cout_mutex.unlock();

        start_signal = true;
        while (winner_id == -1)
        {
            this_thread::yield();
        }

        wins[winner_id]++;
        start_signal = false;
    }

    for (int i = 0; i < number_of_players; i++)
    {
        players[i].join();
    }

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

    thread host_thread(host_function, number_of_players, number_of_rounds);
    host_thread.join();

    return 0;
}