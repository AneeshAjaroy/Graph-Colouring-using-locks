#include <iostream>
#include <fstream>
#include <list>
#include <cstdlib>
#include <time.h>
#include <vector>
#include <map>
#include <thread>
#include <semaphore.h>
#include <sys/time.h>

using namespace std;

int n,k;		// number of vertices,threads(partitions)
struct graph		//graph structure
{
    int vertices;
    list <int> *adjlist;

};
graph *g;
int *colour;
sem_t lock;			//mutex
int maxi =0;			//no. colors


void colorfunc(int a, int b)		//thread function
{
    int start = a;
    int ending = b-1;
    map <int, bool>pos;			//for internal - external partition
    for(int i =start;i<=ending;i++)
    {
        pos[i] = true;
        for(auto j = g->adjlist[i].begin(); j != g->adjlist[i].end(); ++j)
        {
            int num = *j;
            if( (num < start) || (ending < num) )
            {
                pos[i] = false;
                break;
            }
        }
    }
    map<int,bool>m;			//for greedy finding of color
    for(int i =start;i<=ending;i++)
    {
        if(pos[i] == true)
        {

            for(int p =0;p<n;p++)
            {
                m[p] = false;
            }
            for(auto j = g->adjlist[i].begin(); j != g->adjlist[i].end(); ++j)
            {
                if(colour[*j] != -1)
                {
                    m[colour[*j]] = true;
                }
            }
            for(int k =0;k<n;k++)
            {
                if(m[k] == false)
                {
                    colour[i] = k;
                    if(maxi < k)
                    {
                        maxi =k;
                    }
                    break;
                }
            }
            m.clear();
        }else
        {
            sem_wait(&lock); 			//to enter CS
            for(int p =0;p<n;p++)
            {
                m[p] = false;
            }
            for(auto j = g->adjlist[i].begin(); j != g->adjlist[i].end(); ++j)
            {
                if(colour[*j] != -1)
                {
                    m[colour[*j]] = true;
                }
            }
            for(int k =0;k<n;k++)
            {
                if(m[k] == false)
                {
                    colour[i] = k;
                    if(maxi < k)
                    {
                        maxi =k;
                    }
                    break;

                }
            }
            m.clear();
            sem_post(&lock);			//unlocking
        }
    }


}

int main()
{
    char c;
    ifstream in("input_params.txt");		//input file
    in >> k >> n;
    g = new graph;			//graph creation
    g->vertices = n;
    g->adjlist = new list<int>[n];
    in.get(c);
    while(in.get(c))
    {
        if(c == '\n')
        {
            break;
        }
    }
    int temp;
    for(int i =0;i<n;i++)
    {
        in >> temp;
        for(int j =0;j<n;j++)
        {
            in >> temp;
            if(temp == 1)
            {
                g->adjlist[i].push_back(j);
            }
        }
        in.get(c);
    }
    srand(time(NULL));		//seeding
    int part[k+1];		//partition array
    part[0] = 0;
    part[k] = n;
    int i =1;
    int n1 = n;
    while(i < k)
    {
        int num = rand() % n;
        if(num > n1/2)
        {
            i--;
        }
        else
        {
            part[i] = num+ part[i-1];
            n1 = n1-num;
        }
        i++;
    }
    colour = new int[n];
    for(int i =0;i<n;i++)			//initializing mutex locks and colors
    {
        colour[i] = -1;
    }
    sem_init(&lock,0,1);
    vector<thread> threads;
    struct timeval st,en;
    gettimeofday(&st,NULL);

    for(int i=0;i<k;i++)
    {
        threads.push_back(thread(colorfunc,part[i],part[i+1]));		//thread creation
    }
    for(int i=0;i<k;i++)
    {
        threads[i].join();					//thread joining
    }
    gettimeofday(&en,NULL);
    double usec = double( en.tv_usec - st.tv_usec);
    double sec = double(en.tv_sec - st.tv_sec);
    sec = sec*1000000;
    usec = usec + sec;
    usec = usec*0.001;          //time taken


    ofstream out("output.txt");				//output file
    out << "Coarse Lock" <<endl;
    out << "No. of colours used: " << maxi+1<<endl;
    out << "Time taken by the algorithm using: " << usec << " Millisecond"<<endl;
    out << "Colours:" <<endl;

    for(int i=0;i<n-1;i++)
    {
        out << 'v'<<i+1<< " - " << colour[i] + 1<< ", ";

    }
    out << 'v'<<n<< " - " << colour[n-1] + 1;







}
