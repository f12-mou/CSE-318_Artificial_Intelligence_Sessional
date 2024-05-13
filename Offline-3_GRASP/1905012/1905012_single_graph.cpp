#include <bits/stdc++.h>
#include <chrono>
using namespace std;
#define fast ios_base::sync_with_stdio(false),cin.tie(0),cout.tie(0);
#define ll long long
#define pb push_back
#define N 10005

int n,m;                                // number of nodes, number of edges
vector<pair<ll,ll>>adj[N];              // adjacency list
int log_weights[N];                     // weight log
pair<int,int>log_pairs[N];              // pairs log
vector<pair<int, pair<int,int>>>vec;    // weight and pair log
int type;                               // chosen type
double choice;                          // chosen alpha

set<int>best_s1;                        // target set 1
set<int>best_s2;                        // target set 2

int total_itr_local_search=0;           // total number of calls to local_search()
int depthArr[3]= {0,3,7};               // allowed number of iterations


// caching
int lowest_weight;      // highest weight
int highest_weight;     // highest weight

int getFlow(set<int>& s1, set<int>& s2) // returns the total cut value
{
    int total=0;
    map<int,int>who_is_where;
    for(auto& now:s1)
    {
        who_is_where[now]=1;
    }
    for(auto& now:s2)
    {
        who_is_where[now]=2;
    }
    for(auto& now:s1)
    {
        for(auto itr:adj[now])
        {
            int next=itr.first;
            int val=itr.second;
            if(who_is_where[next]==2)
            {
                total+=val;
            }
        }
    }
    return total;
}

int get_binary_search(int threshold_weight)
{
    int low=0, high=vec.size()-1, ans, mid;
    while(low<=high)
    {
        mid=(low+high)/2;
        if(vec[mid].first<threshold_weight)
        {
            low=mid+1;

        }
        else
        {
            ans=mid;
            high=mid-1;
        }
    }
    return ans;
}

void semi_greedy(set<int>&s1, set<int>&s2)
{

    double alpha;
    alpha=choice;
    int threshold_weight =(int)((double)lowest_weight+ (double)(alpha*(double)(highest_weight-lowest_weight)));

    int start_idx_threshold = get_binary_search(threshold_weight);
    int total_elems = (vec.size()-1) - (start_idx_threshold) + 1;
    // random choice
    int random_idx = rand() % total_elems;
    int selected_idx = start_idx_threshold + random_idx;
    s1.insert(log_pairs[selected_idx].first);
    s2.insert(log_pairs[selected_idx].second);

    while(s1.size()+s2.size()!=n)
    {
        set<int>others;
        map<int,int>who_is_where;
        for(auto& now:s1)
        {
            who_is_where[now]=1;
        }
        for(auto& now:s2)
        {
            who_is_where[now]=2;
        }
        for(int i=1; i<=n; i++)
        {
            if(who_is_where[i]==1 || who_is_where[i]==2)
                continue;
            others.insert(i);
        }
        map<int,int>s1_log;
        map<int,int>s2_log;
        int dummy_total=0;
        for(auto& now:others)
        {
            // if I put now in X
            for(int i=0; i<adj[now].size(); i++)
            {
                int neighbor=adj[now][i].first;
                if(who_is_where[neighbor]==2)
                {
                    dummy_total+=(adj[now][i].second);
                }
            }
            s1_log[now]=dummy_total;
            dummy_total=0;
            // if I put now in Y
            for(int i=0; i<adj[now].size(); i++)
            {
                int neighbor=adj[now][i].first;
                if(who_is_where[neighbor]==1)
                {
                    dummy_total+=(adj[now][i].second);
                }
            }
            s2_log[now]=dummy_total;
        }
        vector<pair<int,int>>vec_now; // what_contribution who
        for(auto itr:s1_log)
        {
            vec_now.pb({itr.second, itr.first});
        }
        for(auto itr:s2_log)
        {
            vec_now.pb({itr.second, itr.first});
        }
        sort(vec_now.begin(), vec_now.end());
        int mini_now=vec_now[0].first;
        int maxi_now=vec_now[vec_now.size()-1].first;


        int now=rand()%11;  // produces 0 to 10
        double alpha =((double)now/(double)10.0);
        alpha=choice;
        int threshold_now =(int)((double)mini_now+ (double)(alpha*(double)(maxi_now-mini_now)));

        // binary search
        int low=0, high=vec_now.size()-1, ans, mid;
        while(low<=high)
        {
            mid=(low+high)/2;
            if(vec_now[mid].first<=threshold_now)
            {
                ans=mid;
                low=mid+1;

            }
            else
            {
                high=mid-1;
            }
        }
        // binary search
        int total_elems = (vec_now.size()-1) - (ans) + 1;
        // random choice
        random_idx = rand() % total_elems;
        selected_idx = ans + random_idx;

        int final_selected_elem = vec_now[selected_idx].second;
        if(s1_log[final_selected_elem]>s2_log[final_selected_elem])
        {
            s1.insert(final_selected_elem);
        }
        else
        {
            s2.insert(final_selected_elem);
        }
    }
    best_s1=s1;
    best_s2=s2;// initial best
}


void local_search(set<int>&s1, set<int>&s2, int iterationCount)
{
    if(iterationCount==-1)
        return;
    total_itr_local_search++;
    bool found=false;

    // s1 theke s2 te shorabo
    map<int,int>who_is_where;
    for(auto& now:s1)
    {
        who_is_where[now]=1;
    }
    for(auto& now:s2)
    {
        who_is_where[now]=2;
    }
    int dummy_total=0, dummy_total2=0;
    // for type 3
    vector<pair<int, pair<int,int>>>huhu;// delta who initial set
    int delta;
    for(auto& now:s1)
    {
        dummy_total2=0;
        dummy_total=0;
        for(int i=0; i<adj[now].size(); i++)
        {
            int other=adj[now][i].first;
            int weight=adj[now][i].second;
            if(other==now)
                continue;
            // s2 e thakle ki hoito
            if(who_is_where[other]==1)
            {
                dummy_total2+=weight;
            }
            // s1 e thakle ki hoito
            if(who_is_where[other]==2)
            {
                dummy_total+=weight;
            }
        }
        if(type==1)
        {
            if(dummy_total2>dummy_total)// delta is positive
            {
                found=true;
                s1.erase(now);
                s2.insert(now);
                local_search(s1,s2, iterationCount);
                break;
            }
        }
        else if(type==2)
        {
            if(dummy_total2>=dummy_total)// delta is positive
            {
                found=true;
                s1.erase(now);
                s2.insert(now);
                local_search(s1,s2, iterationCount);
                break;
            }
        }
        else if(type==3)
        {
            delta=dummy_total2-dummy_total;
            huhu.pb({delta, {now, 1}});
        }
    }
    if(found==false || type==3)
    {
        int dummy_total=0, dummy_total2=0;
        for(auto& now:s2)
        {
            dummy_total2=0;
            dummy_total=0;
            for(int i=0; i<adj[now].size(); i++)
            {
                int other=adj[now][i].first;
                int weight=adj[now][i].second;
                if(other==now)
                    continue;
                // s2 e thakle ki hoito
                if(who_is_where[other]==1)
                {
                    dummy_total2+=weight;
                }
                // s1 e thakle ki hoito
                if(who_is_where[other]==2)
                {
                    dummy_total+=weight;
                }
            }
            if(type==1)
            {
                if(dummy_total>dummy_total2)// delta is positive
                {
                    found=true;
                    s2.erase(now);
                    s1.insert(now);
                    local_search(s1,s2, iterationCount);
                    break;
                }
            }
            else if(type==2)
            {
                if(dummy_total>=dummy_total2)// delta is positive
                {
                    found=true;
                    s2.erase(now);
                    s1.insert(now);
                    local_search(s1,s2, iterationCount);
                    break;
                }
            }
            else if(type==3)
            {
                delta=dummy_total-dummy_total2;
                huhu.pb({delta, {now, 2}});
            }
        }
    }
    if(type==3)
    {
        sort(huhu.rbegin(), huhu.rend());

        int nowWho=huhu[0].second.first;
        int initialSet=huhu[0].second.second;
        if(initialSet==1)
        {
            s1.erase(nowWho);
            s2.insert(nowWho);
            // if any update
            if(delta>0)
            {
                best_s1=s1;
                best_s2=s2;
            }
            local_search(s1,s2,iterationCount-1);
        }
        else
        {
            s2.erase(nowWho);
            s1.insert(nowWho);
            // if any update
            if(delta>0)
            {
                best_s1=s1;
                best_s2=s2;
            }
            local_search(s1,s2,iterationCount-1);
        }
    }
}




signed main()
{
    fast

    cout<<"Enter number of nodes "<<endl;
    cin>>n;
    cout<<"Enter number of edges "<<endl;
    cin>>m;
    cout<<"Enter edge details: "<<endl;
    ll u,v,w;
    for(int i=1; i<=m; i++)
    {
        cin>>u>>v>>w;
        adj[u].pb({v,w});
        adj[v].pb({u,w});
        log_pairs[i-1]= {u,v};
        log_weights[i]=w;
        vec.pb({w, {u,v}});
    }
    freopen("depthArr.csv", "w", stdout);
    sort(vec.begin(), vec.end());

    lowest_weight=*min_element(log_weights+1, log_weights+m+1);
    highest_weight=*max_element(log_weights+1, log_weights+m+1);

    cout<<"Alpha,Type,Depth,S1(After Semi-Greedy),S2(After Semi-Greedy),S1,S2,Itr,Total, Time(ms)"<<endl;
    for(double i=0.0; i<=1.0; i+=0.1)
    {
        choice=i;
        for(int j=1; j<=3; j++)
        {
            for(int k=1; k<=2; k++)
            {
                type=j;
                total_itr_local_search=0;
                set<int>s1, s2;
                auto beginTime = std::chrono::high_resolution_clock::now();
                semi_greedy(s1,s2);
                cout<<choice<<","<<type<<","<<depthArr[k]<<",";
                cout<<"(";
                for(auto& now:s1)
                {
                    cout<<now<<" ";
                }
                cout<<"),";
                cout<<"(";
                for(auto& now:s2)
                {
                    cout<<now<<" ";
                }
                cout<<"),";
                local_search(s1,s2,depthArr[k]);
                auto endTime = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - beginTime);
                if(type!=3)
                {
                    best_s1=s1;
                    best_s2=s2;
                }
                cout<<"(";
                for(auto& now:best_s1)
                {
                    cout<<now<<" ";
                }cout<<"),";
                cout<<"(";
                for(auto& now:best_s2)
                {
                    cout<<now<<" ";
                }
                cout<<" ),";
               cout<<total_itr_local_search<<",";
               cout<<getFlow(s1,s2)<<",";
               cout<<(elapsed.count() * 1e-9)*1000<<endl;
            }
        }
    }
    return 0;
}
/*
5
7
1 2 5
1 5 6
2 5 10
2 3 9
3 5 2
4 5 10
3 4 15
*/
/*
10 20
6 9 8
4 7 1
9 10 8
5 6 7
4 9 2
1 2 5
8 9 8
6 8 8
7 9 2
6 10 1
1 4 6
2 3 2
4 5 7
1 5 5
3 4 7
7 8 1
6 7 8
7 10 3
8 10 7
5 10 3
*/
