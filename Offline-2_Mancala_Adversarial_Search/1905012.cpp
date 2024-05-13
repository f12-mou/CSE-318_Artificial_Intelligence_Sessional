#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define fast ios_base::sync_with_stdio(false),cin.tie(0),cout.tie(0);
#define MINI 1
#define MAXI 2
#define MINI_HOME 1
#define MAXI_HOME 8
#define MINI_Start 9
#define MAXI_Start 2
#define MINI_End 14
#define MAXI_End 7
#define MINUS_INFINITY -10000
#define PLUS_INFINITY 10000

const int W1 = 19, W2 = 17, W3 = 7, W4 = 3;

int frontArr[15]={0, 8, 14, 13, 12, 11, 10, 9, 1, 7, 6, 5, 4, 3, 2};
int nowDepth=1;
int MAXI_heuristic=1;
int MINI_heuristic=1;

class state
{
public:
    int a[15];
    int who;                    // Min or Max
    int totalRemStones[3];      // Total Remaining Stones in index 1 and 2
    int totalStonesInHome[3];   // Total achieved Stones in index 1 and 2
    int extraMoves[3];          // Total extra moves in index 1 and 2
    int extraCaptured[3];       // Total extra captured in index 1 and 2
    int myOrigin;               // my optimal bin to go
    void changeIdentity()       // To deliver the current state to opposite player
    {
        if(who == MINI) who = MAXI;
        else    who = MINI;
    }
    void initialize()           // Initialize the game state
    {
        for(int i=1;i<=14;i++)
            a[i]=4;
        a[MINI_HOME]=0;
        a[MAXI_HOME]=0;
        totalRemStones[1]=totalRemStones[2]=24;     // 6*4=24
        totalStonesInHome[1]=totalStonesInHome[2]=0;
        myOrigin=0;
        for(int i=1;i<=2;i++)
        {
            extraCaptured[i]=0;
            extraMoves[i]=0;
        }
        who=MAXI;
    }
    void print()
    {
        cout<<endl;
        for(int i=1;i<=14;i++)
        {
            cout<<a[i]<<" ";
            if(i== 1 || i==7 || i==8)
            {
                cout<<"     ";
            }
        }
        cout<<endl;
        cout<<endl;
    }
};

class Game
{
public:
    ll heuristic_1(state initial)
    {
        //(stones_in_my_storage – stones_in_opponents_storage)
        initial=updateRemBins(initial);
        ll res = initial.a[MAXI_HOME]-initial.a[MINI_HOME];
        return res;
    }
    ll heuristic_2(state initial)
    {
        //W1 * (stones_in_my_storage – stones_in_opponents_storage) + W2 * (stones_on_my_side – stones_on_opponents_side)
        initial=updateRemBins(initial);
        ll res = W1 * (initial.a[MAXI_HOME]-initial.a[MINI_HOME]) + W2 * (initial.totalRemStones[MAXI_HOME]-initial.totalRemStones[MINI_HOME]);
        return res;
    }
    ll heuristic_3(state initial)
    {
        //W1 * (stones_in_my_storage – stones_in_opponents_storage) + W2 * (stones_on_my_side – stones_on_opponents_side) + W3 * (additional_move_earned)
        initial=updateRemBins(initial);
        ll res = W1 * (initial.a[MAXI_HOME]-initial.a[MINI_HOME]) + W2 * (initial.totalRemStones[MAXI_HOME]-initial.totalRemStones[MINI_HOME])
                    + W3 * initial.extraMoves[MAXI];
        return res;
    }
    ll heuristic_4(state initial)
    {
        // W1 * (stones_in_my_storage – stones_in_opponents_storage) + W2 * (stones_on_my_side – stones_on_opponents_side)
        // + W3 * (additional_move_earned) + W4 * (stones_captured)
        initial=updateRemBins(initial);
        ll res = W1 * (initial.a[MAXI_HOME]-initial.a[MINI_HOME]) + W2 * (initial.totalRemStones[MAXI_HOME]-initial.totalRemStones[MINI_HOME])
                    + W3 * initial.extraMoves[MAXI] + W4 * initial.extraCaptured[MAXI];
        return res;
    }
    ll getBeta(state initial)
    {
        initial=updateRemBins(initial);
        if(MINI_heuristic == 1) return heuristic_1(initial);
        else if(MINI_heuristic == 2) return heuristic_2(initial);
        else if(MINI_heuristic == 3) return heuristic_3(initial);
        else if(MINI_heuristic == 4) return heuristic_4(initial);
    }
    ll getAlpha(state initial)
    {
        initial=updateRemBins(initial);
        if(MAXI_heuristic == 1) return heuristic_1(initial);
        else if(MAXI_heuristic == 2) return heuristic_2(initial);
        else if(MAXI_heuristic == 3) return heuristic_3(initial);
        else if(MAXI_heuristic == 4) return heuristic_4(initial);
    }

    bool isInOpponentTerritory(int who, int bin)
    {
        if(who==MAXI && (MINI_Start<=bin && bin<=MINI_End))
        {
            return true;
        }
        else if(who==MINI && (MAXI_Start<=bin && bin<=MAXI_End))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    state updateRemBins(state initial) // Based on the current situation of the array, other parameters are updated.
    {
        int total=0;
        state retState = initial;
        for(int i=MINI_Start;i<=MINI_End;i++)
        {
            total += initial.a[i];
        }
        retState.totalRemStones[MINI] = total;
        total=0;
        for(int i=MAXI_Start; i<=MAXI_End; i++)
        {
            total += initial.a[i];
        }
        retState.totalRemStones[MAXI]= total;                   // remaining stones updated
        retState.totalStonesInHome[MINI]=initial.a[MINI_HOME];  // in storage stones updated
        retState.totalStonesInHome[MAXI]=initial.a[MAXI_HOME];
        return retState;
    }
    state getState(state initial, int bin, int alphagot, int betagot, int depth)// input: initial , ke khelbe thik kora: output: modify kore abar kar haate khela dibo updated state
    {
        state childState = initial;
        int nowStones = childState.a[bin];
        int start=(bin+1)%14;       // from where stone dropping will be done
        if(start==0) start=14;
        int oppositeHome;
        int myHome;
        if(childState.who == MINI)
        {
            oppositeHome = MAXI_HOME;
            myHome = MINI_HOME;
        }
        else
        {
            oppositeHome = MINI_HOME;
            myHome = MAXI_HOME;
        }
        if(bin==0) bin=14;
        childState.a[bin]=0;        // Picked up all the stones
        for(int i=1;i<=nowStones;i++)
        {
            if(start == oppositeHome)   // making sure I am not throwing stone in my opponent's storage
            {
                start=(start+1)%14;
                if(start==0) start=14;
            }
            childState.a[start]++;          // Dropped in the location
            int nowAlpha = alphagot, nowBeta = betagot;
            if(i==nowStones)
            {
                // I dropped my last Stone
                if(isInOpponentTerritory(childState.who, start))
                {
                    // no extra moves
                    childState=updateRemBins(childState);
                    childState.changeIdentity();
                    return childState;
                }
                else if(start == myHome)
                {
                    state nowState = childState;
                    nowState.extraMoves[nowState.who]++;    // extra moves earned
                    nowState= updateRemBins(nowState);      // prepared for depth first search
                    nowState = dfs(nowState, nowAlpha, nowBeta, depth);
                    int optimal_bin=nowState.myOrigin;
                    if(optimal_bin==-2 || optimal_bin == -1)     // Though this case can't arise, better to be written
                    {
                        nowState=updateRemBins(nowState);
                        nowState.changeIdentity();
                        return nowState;
                    }
                    state retState = getState(nowState, optimal_bin, alphagot, betagot, nowDepth);// nowState optimal_bin_no diye khele final ki obostha daray ta return dibe with who updated
                    return retState;
                }
                else
                {
                    if(childState.a[start]==1 && start!=myHome) // It was empty bin
                    {
                        childState.a[myHome]+=(childState.a[start]+childState.a[frontArr[start]]);// amar r opponent er shob nilam
                        childState.extraCaptured[childState.who] += childState.a[frontArr[start]];
                        childState.a[start]=0;
                        childState.a[frontArr[start]]=0;
                        childState=updateRemBins(childState);
                        childState.changeIdentity();
                        return childState;
                    }
                    else // it was actually not empty so do nothing
                    {
                        childState=updateRemBins(childState);
                        childState.changeIdentity();
                        return childState;
                    }
                }
            }
            start=(start+1)%14;
            if(start==0) start=14;
        }
    }
    int opponent(int who)
    {
        if(who==MINI)   return MAXI;
        else    return MINI;
    }
    state dfs(state initial, int& alpha, int& beta, int depth)
    {
        if(initial.totalRemStones[initial.who]==0) // I have nothing to do, opponent has to end the game by capturing every stone
        {
            state ansState = initial;
            if(initial.who == MINI)
            {
                ansState.totalStonesInHome[MAXI] += ansState.totalRemStones[MAXI];
                ansState.totalRemStones[MAXI]=0;
                ansState.myOrigin=-1;
                for(int i=1;i<=14;i++)
                {
                    if(i!=MINI_HOME && i!=MAXI_HOME) ansState.a[i]=0;
                }
                beta = getBeta(initial);
                return ansState;
            }
            else
            {
                ansState.totalStonesInHome[MINI]+=ansState.totalRemStones[MINI];
                ansState.totalRemStones[MINI]=0;
                ansState.myOrigin=-1;
                for(int i=1;i<=14;i++)
                {
                    if(i!=MINI_HOME && i!=MAXI_HOME) ansState.a[i]=0;
                }
                alpha = getAlpha(initial);
                return ansState;
            }
        }

        if(initial.totalRemStones[opponent(initial.who)]==0) // Yes I have to end the game !
        {
            state ansState = initial;
            if(initial.who == MINI)
            {
                ansState.totalStonesInHome[MINI] += ansState.totalRemStones[MINI];
                ansState.totalRemStones[MINI]=0;
                ansState.myOrigin=-2;
                //beta = ansState.totalStonesInHome[MAXI]-ansState.totalStonesInHome[MINI];
                for(int i=1;i<=14;i++)
                {
                    if(i!=MINI_HOME && i!=MAXI_HOME) ansState.a[i]=0;
                }
                beta = getBeta(ansState);
                return ansState;
            }
            else
            {
                ansState.totalStonesInHome[MAXI]+=ansState.totalRemStones[MAXI];
                ansState.totalRemStones[MAXI]=0;
                ansState.myOrigin=-2;
                //alpha = ansState.totalStonesInHome[MAXI]-ansState.totalStonesInHome[MINI];
                for(int i=1;i<=14;i++)
                {
                    if(i!=MINI_HOME && i!=MAXI_HOME) ansState.a[i]=0;
                }
                alpha = getAlpha(ansState);
                return ansState;
            }
        }

        if(depth==0)
        {
            state ansState=initial;
            if(initial.who == MINI)
            {
                ansState.myOrigin=-3;
                beta = getBeta(initial);
                return ansState;
            }
            else // MAXI
            {
                ansState.myOrigin=-3;
                alpha = getAlpha(initial);
                return ansState;
            }
        }
        bool started = false;
        state ansState;
        int optimal_bin=-1;
        if(initial.who == MAXI) // Max will play
        {
            int nowAlpha = alpha, nowBeta=beta;
            for(int i = MAXI_Start; i<=MAXI_End; i++)
            {
                if(initial.a[i]==0) continue;
                if(started == true) // maxi er kache valid akta alpha already ase
                {
                    state childState= getState(initial, i, alpha, beta, depth);
                    state found = dfs(childState, nowAlpha, nowBeta, depth-1);
                    int found_new_alpha = nowBeta;
                    if(found_new_alpha>nowAlpha)
                    {
                        nowAlpha=found_new_alpha;
                        optimal_bin=i;
                    }
                }
                else
                {
                    started = true;
                    state childState= getState(initial, i, alpha, beta, depth);
                    childState = dfs(childState, nowAlpha, nowBeta, depth-1); // child to minimum, so it will give val to beta
                    nowAlpha = nowBeta;
                    optimal_bin=i;
                }

                if(nowAlpha>=beta)
                {
                    break;
                }
            }
            initial.myOrigin=optimal_bin; // optimal bin detected
            alpha=nowAlpha;
            return initial;
        }
        else
        {
            int nowBeta=beta, nowAlpha=alpha;
            for(int i = MINI_Start; i<=MINI_End; i++)
            {
                if(initial.a[i]==0) continue;
                if(started == true) // maxi er kache valid akta alpha already ase
                {
                    state childState= getState(initial, i, alpha, beta, depth);
                    childState = dfs(childState, nowAlpha, nowBeta, depth-1);
                    int found_a_possible_beta=nowAlpha;
                    if(found_a_possible_beta<nowBeta)
                    {
                        nowBeta=found_a_possible_beta;
                        optimal_bin=i;
                    }
                }
                else
                {
                    started = true;
                    state childState= getState(initial, i, alpha, beta, depth);
                    childState = dfs(childState, nowAlpha, nowBeta, depth-1); // child to maxi, so it will give val to alpha
                    nowBeta = nowAlpha;
                    optimal_bin=i;
                }
                if(alpha>=nowBeta)
                {
                    break;
                }
            }
            initial.myOrigin=optimal_bin;
            beta=nowBeta;
            return initial;
        }
    }

    bool nowCondition(state& initial, int bin)
    {
        state ansState=initial;
        int myHome, oppoHome, isInmyHome, startIdx;
        if(ansState.who==MINI)
        {
            myHome=MINI_HOME;
            oppoHome=MAXI_HOME;
            startIdx=bin;
        }
        else
        {
            myHome=MAXI_HOME;
            oppoHome=MINI_HOME;
            startIdx=bin;
        }
        int total=initial.a[bin];
        initial.a[bin]=0;
        startIdx=(startIdx+1)%14;
        if(startIdx==0) startIdx=14;
        for(int i=1;i<=total;i++)
        {
            if(startIdx==oppoHome)
            {
                startIdx=(startIdx+1)%14;
                if(startIdx==0) startIdx=14;
            }
            initial.a[startIdx]++;
            if(i==total)
            {
                if(isInOpponentTerritory(initial.who, startIdx))
                {
                    // no extra moves
                    initial=updateRemBins(initial);
                    return false;
                }
                else if(startIdx == myHome)
                {
                   initial.extraMoves[initial.who]++;
                   initial= updateRemBins(initial);
                    return true;

                }
                else
                {
                    if(initial.a[startIdx]==1 && startIdx!=myHome) // It was empty
                    {
                        initial.a[myHome]+=(initial.a[startIdx]+initial.a[frontArr[startIdx]]);// amar r opponent er shob nilam
                        initial.extraCaptured[initial.who] += initial.a[frontArr[startIdx]];
                       initial.a[startIdx]=0;
                       initial.a[frontArr[startIdx]]=0;
                       initial=updateRemBins(initial);
                       return false;

                    }
                    else // it was actually not empty
                    {
                        initial=updateRemBins(initial);
                        return false;
                    }
                }
            }
                startIdx=(startIdx+1)%14;
                if(startIdx==0) startIdx=14;
        }
    }
};


signed main()
{
    fast
    while(1)
    {
        cout<<"Welcome to Mancala......................"<<endl;
        cout<<"Enter option: "<<endl;
        cout<<"1. AI vs AI"<<endl;
        cout<<"2. You vs AI"<<endl;
        int option;
        cin>>option;
        int depthChoice, heuristics1, heuristics2;
        cout<<"Enter the depth : "<<endl;
        cin>>depthChoice;
        nowDepth=depthChoice;
        if(option==1){
        cout<<"Enter heuristic for MAX player: "<<endl;
        cin>>heuristics1;

        }
        cout<<"Enter heuristic for MIN player: "<<endl;
        cin>>heuristics2;

        if(option==1) // AI vs AI
        {
            MAXI_heuristic = heuristics1;
            MINI_heuristic = heuristics2;
        }
        else
        {
            MAXI_heuristic = MINI_heuristic = heuristics2;
        }




        if(option==1)
        {
            Game game;
            state initial;
            initial.initialize();
            int alpha = MINUS_INFINITY, beta = PLUS_INFINITY;
            int start=2;
            while(1)
            {
                if(start==2)
                {
                    initial.who=start;
                    cout<<"The game will be played by MAX with the following situation:"<<endl;
                    initial.print();
                    state nowStateGame = game.dfs(initial, alpha, beta,nowDepth); // To know what is my optimal move
                    if(nowStateGame.myOrigin>0)
                        cout<<"For MAX player, the optimal move is bin number: "<<nowStateGame.myOrigin<<endl;
                    if(nowStateGame.myOrigin<0)
                    {
                        if(nowStateGame.totalStonesInHome[MAXI]>=nowStateGame.totalStonesInHome[MINI])
                            cout<<"First player(MAXI) wins"<<endl;
                        else
                            cout<<"Second player(MINI) wins"<<endl;

                        cout<<"Score (MAXI): "<<nowStateGame.totalStonesInHome[MAXI]<<endl;
                        cout<<"Score (MINI): "<<nowStateGame.totalStonesInHome[MINI]<<endl;
                        cout<<endl;
                        break;
                    }
                    bool ou=game.nowCondition(initial, nowStateGame.myOrigin);
                    if(ou==true)
                    {
                        // claim another move
                    }
                    else
                    {
                        start=1;
                    }
                    cout<<"Player number : "<<start<<" will play now"<<endl;
                }
                else
                {
                    initial.who=start;
                    cout<<"The game will be played by MINI with the following situation:"<<endl;
                    initial.print();
                    state nowStateGame = game.dfs(initial, alpha, beta, nowDepth); // To know what is my optimal move
                    if(nowStateGame.myOrigin>0)
                        cout<<"For MIN player, the optimal move is bin number: "<<nowStateGame.myOrigin<<endl;
                    if(nowStateGame.myOrigin<0)
                    {
                        if(nowStateGame.totalStonesInHome[MAXI]>=nowStateGame.totalStonesInHome[MINI])
                            cout<<"First player(MAXI) wins"<<endl;
                        else
                            cout<<"Second player(MINI) wins"<<endl;

                        cout<<"Score (MAXI): "<<nowStateGame.totalStonesInHome[MAXI]<<endl;
                        cout<<"Score (MINI): "<<nowStateGame.totalStonesInHome[MINI]<<endl;
                        cout<<endl;
                        break;
                    }
                    bool ou=game.nowCondition(initial, nowStateGame.myOrigin);
                    if(ou==true)
                    {
                        // claim again
                        // do not give to other
                    }
                    else
                    {
                        start=2;
                    }
                    cout<<"Player number : "<<start<<" will play now"<<endl;
                }
            }
        }
        else
        {
            Game game;
            state initial;
            initial.initialize();
            int alpha = MINUS_INFINITY, beta = PLUS_INFINITY;
            int start=2;
            cout<<endl;
            cout<<"Best of Luck !"<<endl;
            while(1)
            {
                initial=game.updateRemBins(initial);
                if(initial.totalRemStones[MAXI]==0 && initial.totalRemStones[MINI]==0)
                {
                    if(initial.totalStonesInHome[MAXI]>=initial.totalStonesInHome[MINI])
                    {
                        cout<<"You won !!!" <<endl;
                    }
                    else
                    {
                        cout<<"You lose ______"<<endl;
                    }
                    cout<<"Your Score: "<<initial.totalStonesInHome[MAXI]<<endl;
                    cout<<"Your opponent's score: "<<48-initial.totalStonesInHome[MAXI]<<endl;
                    cout<<endl;
                    break;
                }
                else if(initial.totalRemStones[MAXI]==0)
                {
                    if(initial.totalStonesInHome[MAXI]>=initial.totalStonesInHome[MINI]+initial.totalRemStones[MINI])
                    {
                        cout<<"You won !!!" <<endl;
                    }
                    else
                    {
                        cout<<"You lose ______"<<endl;
                    }
                    cout<<"Your Score: "<<initial.totalStonesInHome[MAXI]<<endl;
                    cout<<"Your opponent's score: "<<48-initial.totalStonesInHome[MAXI]<<endl;
                    cout<<endl;
                    break;
                }
                else if(initial.totalRemStones[MINI]==0)
                {
                    if(initial.totalStonesInHome[MAXI]+initial.totalRemStones[MAXI]>=initial.totalStonesInHome[MINI])
                    {
                        cout<<"You won !!!" <<endl;
                    }
                    else
                    {
                        cout<<"You lose ______"<<endl;
                    }
                    cout<<"Your Score: "<<48-initial.totalStonesInHome[MINI]<<endl;
                    cout<<"Your opponent's score: "<<initial.totalStonesInHome[MINI]<<endl;
                    cout<<endl;
                    break;
                }


                if(start==2)
                {
                    initial.who=start;
                    cout<<"The game will be played by you with the following situation:"<<endl;
                    initial.print();
                    int origin;
                    cout<<"Enter your option with any bin (between 2 and 7 inclusive) with at least one stone: "<<endl;
                    cin>>origin;
                    if(2<=origin  && origin<=7)
                        cout<<"Your move is bin number: "<<origin<<endl;

                    bool ou=game.nowCondition(initial, origin);
                    if(ou==true)
                    {
                        // claim another move
                    }
                    else
                    {
                        start=1;
                    }
                    cout<<"Player number : "<<start<<" will play now"<<endl;
                }
                else
                {
                    initial.who=start;
                    cout<<"The game will be played by MINI with the following situation:"<<endl;
                    initial.print();
                    state nowStateGame = game.dfs(initial, alpha, beta, nowDepth);
                    if(nowStateGame.myOrigin>0)
                        cout<<"For MIN player, the optimal move is bin number: "<<nowStateGame.myOrigin<<endl;
                    bool ou=game.nowCondition(initial, nowStateGame.myOrigin);
                    if(ou==true)
                    {
                        // claim again
                        // do not give to other
                    }
                    else
                    {
                        start=2;
                    }
                    cout<<"Player number : "<<start<<" will play now"<<endl;
                }
            }
        }

    }
    return 0;
}



