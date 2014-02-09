#include "iostream"
#include "stdio.h"
#include "algorithm"
#include "math.h"
#include "string.h"
#include "time.h"
#include "stdlib.h"
#include "vector"
#include "queue"
#define MALLOC(name,type,n) (name=(type*)malloc(sizeof(type)*(n)))
#define INF 123456732
using namespace std;
typedef long long int ll;
#define intree(v) tobj.in[v.x][v.y]

//including all types
typedef struct pixel
{
	int r,g,b;
}pixel;

typedef struct vertex
{
	int x,y;	
}vertex;

typedef struct edge
{
	vertex v1,v2;
	int c;	
}edge;
typedef struct node//type for tree node
{
	int link[10];
	/*this is a way to represnt if the neighbour of a particualr node is present in tree
	if node is (x,y) then link[0] reprents if (x-1,y-1) is connected to this or not
	similarly (x+1,y+1)'s presence is stored in link[8]
	*/
}node;
int wd,ht;//width and height of image

class image
{
	//image storage class
public:
	int ht,wd;
	pixel** data;
	void init(int a,int b)
	{
		//images of wd and ht pixels
		MALLOC(data,pixel*,a+1);
		for (int i = 0; i < a; ++i)
			MALLOC(data[i],pixel,b+1);
			ht=a,wd=b;		
	}
	void output()
	{
		//printing the ppm file
		printf("P3\n%d %d\n",wd,ht);
		printf("255\n");
		for (int i = 0; i < ht; ++i)
			for (int j = 0; j < wd; ++j)
			{
					printf("%d %d %d\n",this->data[i][j].r,this->data[i][j].g,this->data[i][j].b );
			}
	}
}img1,fimg;//img1--original image
//fimg final segmented image


//basic functions for all classes
pixel img(vertex x)
{
	//for easy access like img(v)
	return img1.data[x.x][x.y];
}
pixel img(int x,int y)
{
	//for easy access like img[a][b]
	return img1.data[x][y];
}

//three overloaded functions for calcalting cost of edge for two pixels when pixels,edge and vertices are given
int cost(pixel a,pixel b)
{
	return abs(a.r-b.r)+abs(a.g-b.g)+abs(a.b-b.b);
}
int cost(edge x)
{
	pixel a=img(x.v1),b=img(x.v2);
	return cost(a,b);
}
int cost(vertex a,vertex b)
{
	pixel a1=img(a),b1=img(b);
	return cost(a1,b1);
}


class maxedge;
extern maxedge maxobj;


class treeclass//for objects of Mst tree
{
public:
	node** tree;
	bool **in;//stores if a vertex v is present in tree
	int np;//number of vertices in set P;
	void init()
	{
		//tree of size of img and also bool is initialized(memory)
		MALLOC(tree,node*,ht+1);
		MALLOC(in,bool*,ht+1);
		for (int i = 0; i < ht; ++i){
			MALLOC(tree[i],node,wd+1);
			MALLOC(in[i],bool,wd+1);
		}

		for (int i = 0; i < ht; ++i){
			for (int j = 0; j < wd; ++j){
				in[i][j]=0;//initially no vertex is present in tree
				for (int k = 0; k < 9; ++k){
					tree[i][j].link[k]=0;//no connections between any two edges
				}
				
			}
		}
		in[0][0]=1;//adding (0,0) vertex to tree
		np=1;//making number of vertices present in tree as 1
	}
	int canadd(edge e)//if edge can be added to the tree
	{
		if(in[e.v1.x][e.v1.y]&&in[e.v2.x][e.v2.y])//if both vertices are present already in tree can't add to tree
			return 0;
		return 1;
	}
	int index(vertex v1,vertex v2)
	{
		//find the link index for these two vertices
		//index for v1 as centre vertex
		int x1=v1.x,y1=v1.y,x2=v2.x,y2=v2.y;
		int count=-1;
		for (int i = -1; i <=1 ; ++i){
			for (int j = -1; j <= 1 ; ++j){
				count++;
				if(x1+i<0||x1+i>=ht||y1+j<0||y1+j>=wd||(i==0&&j==0))
					continue;
				if(x1+i==x2&&y1+j==y2)
					return count;
			}
		}
	}
	int insert(edge e)
	{
		if(!canadd(e))//if both vertices of e are present in tree already neglect it
			return 0;

		vertex v1=e.v1,v2=e.v2;
		int nbrc=index(v1,v2);
		tree[v1.x][v1.y].link[nbrc]=1;//adding branch from (x1,y1) to (x2,y2)
		tree[v2.x][v2.y].link[8-nbrc]=1;//adding branch from (x2,y2) to (x1,y1)

		np++;

		in[v1.x][v1.y]=1;//updating in for both the vertices
		in[v2.x][v2.y]=1;
		return 1;
	}
	void print()
	{
		//printing the mst
		for (int i = 0; i < ht; ++i){
			for (int j = 0; j < wd; ++j){
				printf("%d %d--  ",i,j );
				for (int k = 0; k < 9; ++k){
					if(tree[i][j].link[k]==1)
						printf("%d  ",k );
				}
				printf("\n");
			}
		}
	}
	void del(edge e)
	{
		//to delete a link between two vertices
		//for segmenting the tree into s parts 
		int count=index(e.v1,e.v2);
		tree[e.v1.x][e.v1.y].link[count]=0;
		tree[e.v2.x][e.v2.y].link[8-count]=0;
	}
}tobj;//tree object
vertex ntv(edge e)//non tree vertex
{
	//returns if a given vertex is present in tree
	if(intree(e.v1))
		return e.v2;
	return e.v1;
}

class minheap
{
	#define qi(v) qi[v.x][v.y]
public:
	std::vector<edge> heap;
	int n;
	int** qi;
	void init()
	{
		heap.resize(ht*wd*10);
		MALLOC(qi,int*,ht+1);
		for (int i = 0; i < ht; ++i){
			MALLOC(qi[i],int,wd+1);
		}
		for (int i = 0; i < ht; ++i){
			for (int j = 0; j < wd; ++j){
				qi[i][j]=-1;//vertex i,j not in heap
			}
		}
		vertex zero={0,0},a={0,1},b={1,0},c={1,1};
		edge e1={zero,a,cost(a,zero)},e2={zero,b,cost(b,zero)},e3={zero,c,cost(c,zero)};
		insert(e1);
		insert(e2);
		insert(e3);
		n=3;
	}
	void insert(edge e)
	{
		if(!tobj.canadd(e))
			return;
		if(qi(ntv(e))!=-1)//already in heap
		{
			changepriority(e);
			return;
		}
		/*printf("adding to heap with  ");
		printedge(e);*/
		e.c=cost(e);
		heap[n]=e;

		int i=n,p;
		while(i>0)
		{
			p=(i-1)/2;
			if(p<0)
				break;
			if(heap[p].c<=heap[i].c)
				break;
			qi(ntv(heap[i]))=p;
			qi(ntv(heap[p]))=i;
			swap(heap[i],heap[p]);
			i=p;
		}
		n++;
	}
	edge del()
	{
		if(n==0)
		{
			printf("deled all");
		}
		edge t=heap[0];
		heap[0]=heap[n-1];
		n--;
		/*printf("deleted edge ");
		printedge(t);*/
		int i=0,c1,c2,min;
		while(i<n)
		{
			c1=2*i+1;
			c2=c1+1;
			if(c1>=n)
				break;
			if(c1==n)
				min=c1;
			else if(heap[c1].c<=heap[c2].c)
				min=c1;
			else
				min=c2;
			if(heap[i].c<=heap[min].c)
				break;
			qi(ntv(heap[i]))=min;
			qi(ntv(heap[min]))=i;
			swap(heap[i],heap[min]);
			i=min;
		}
		return t;
	}
	void changepriority(edge e)
	{

		vertex v=ntv(e);
		int index=qi(v);

		if(heap[index].c<=e.c)
			return;
		/*printf("changing priority --");
		printedge(e);*/
		heap[index]=e;

		int i=index,p;
		while(i>0)
		{
			p=(i-1)/2;
			if(p<0)
				break;
			if(heap[p].c<=heap[i].c)
				break;
			qi(ntv(heap[i]))=p;
			qi(ntv(heap[p]))=i;
			swap(heap[i],heap[p]);
			i=p;
		}
	}

}hobj;
class maxedge
{
	/*
	for obtaining the maximum s links of the MST we can maintain a min heap of some N vertices such that for all s suc that s<N 
	if an edge is added to MST while creating it and it is greater than minimum edge this minheap then delete minumum
	add that vertex to minheap
	finally sort the minheap of N vertices descendingly
	*/
public:
	std::vector<edge> vec;
	int n;
	maxedge()
	{
		n=100;
		vec.resize(100);
		for (int i = 0; i < 100; ++i)
			vec[i].c=-INF;//initailizing all queue elements to -INF
	}
	void insert(edge x)
	{
		if(x.c<this->vec[0].c)//if the new edge is less than minimum of all edges present in minqueue dont add it
			return;
		this->vec[0]=x;
		heapify();
	}
	edge deleteq()
	{
		edge x=this->vec[0];
		this->vec[0]=this->vec[n-1];
		n--;
		heapify();
		return x;
	}
	void heapify()
	{
		int i,l,r,min;
		i=0;
		while(i<n) {
			l=2*i+1,r=2*i+2;
			if(l>=n)return;
			if(l==n-1)
				min=l;
			else if(this->vec[l].c<this->vec[r].c)
				min=l;
			else
				min=r;
			if(this->vec[i].c<=this->vec[min].c)
				break;

			swap(this->vec[i],this->vec[min]);
			i=min;
		}
	}
	static int compare(edge a,edge b)
	{
		if(a.c>b.c)
			return 1;
		return 0;
	}
	void sortedge()//sorting minheap edges
	{
		sort(this->vec.begin(),this->vec.end(),compare);
	}
	void print()
	{
		for (int i = 0; i < 100; ++i){
			printf("%d \n",vec[i].c );
		}
	}
};
maxedge maxobj;
class bfs
{
	//for bfs traversal in segmented tree and coloring each segment
public:
	int color;//color variable for each segment
	int c,s;
	queue <vertex> cutver;//queue to store vertices when a link is broken from MST
	queue <vertex> colorq;//queue to traverse each and every segment and color it
	bool **iscolored;//wheter a vertex is already colored or not
	bfs()
	{
		MALLOC(iscolored,bool*,ht+1);
		for (int i = 0; i < ht; ++i){
			MALLOC(iscolored[i],bool,wd+1);
		}
		for (int i = 0; i < ht; ++i){
			for (int j = 0; j <wd ; ++j){
				iscolored[i][j]=0;
			}
		}
		color=0;
		c=0;
	}
	void dissect(int y)//segmenting MST into y+1 parts
	{
		for (int i = 0; i < y; ++i){
			edge e=maxobj.vec[i];
			cutver.push(e.v1);
			cutver.push(e.v2);
			tobj.del(e);
		}
	}
	pixel getpixel()//getting color for a color variable
	{
		pixel p[]={{0,0,0},{255,0,0},{0,255,0},{0,0,255},{255,255,0},{0,255,255},{255,0,255},{25,25,25},{122,55,139},
					{46,139,87},{139,69,0},{123,45,235},{234,54,231},{98,123,87},{15,200,198},{184,237,9}};
		return p[color%16];

	}
	void segment()
	{
		//segemting and coloring function
		while(!cutver.empty())//till all segments are over
		{
			vertex f=cutver.front();
			cutver.pop();
			if(iscolored[f.x][f.y])//is already colored neglect
				continue;
			colorq.push(f);
			color++;
			c++;
			while(!colorq.empty()) {//color all vertices in that segment with same color
				vertex v=colorq.front();
				colorimg(v);
				colorq.pop();
				iscolored[v.x][v.y]=1;


				int x1=v.x,y1=v.y;
				int count=-1;
				for (int i = -1; i <=1 ; ++i){
					for (int j = -1; j <= 1 ; ++j){
						count++;
						if(x1+i<0||x1+i>=ht||y1+j<0||y1+j>=wd||(i==0&&j==0))
							continue;
						if(tobj.tree[x1][y1].link[count]&&!iscolored[x1+i][y1+j])	
						{
							vertex p={x1+i,y1+j};//maintaing the queue
							colorq.push(p);
						}
					}
				}

			}
		}
	}

	void colorimg(vertex x)
	{
		//color vertx x in new image
		fimg.data[x.x][x.y]=getpixel();
	}
};
void input();
void getmst();
int main()
{
	freopen("original.ppm","r",stdin);//opeing original.ppm in stdin
	freopen("output.ppm","w",stdout);//output in stdout
	input();
	FILE* fp=fopen("output","w");//for any outputs we can use this fp
	tobj.init();//initializing both tree and minheap
	hobj.init();
	getmst();//mst done

	maxobj.sortedge();//sorting the maxedge heap
	fimg.init(ht,wd);//initializing output image

	bfs seg;
	seg.dissect(16);//segmentting into 16 parts
	seg.segment();//coloring
	fimg.output();//printing image
	return 0;
}
void input()
{
	//taking image in ppm format
	char temp[100];
	int maxvl;
	scanf(" %s ",temp);//taking the magic number
	scanf(" %d %d %d ",&wd,&ht,&maxvl);
	img1.init(ht,wd);

	for (int i = 0; i < ht; ++i){
		for (int j = 0; j < wd; ++j){
			scanf(" %d ",&(img1.data[i][j].r));
			scanf(" %d ",&(img1.data[i][j].g));
			scanf(" %d ",&(img1.data[i][j].b));
		}
	}
}
void getmst()
{
	//prims algorithm
	while(tobj.np<=ht*wd&&hobj.n>0) {
		edge de=hobj.del();
		vertex nontv=ntv(de);
		int r=tobj.insert(de);
		if(r==1)
			maxobj.insert(de);
		int x=nontv.x,y=nontv.y;
		edge e;
		vertex v1,v2;
		for (int i = -1; i <= 1; ++i){
			for (int j = -1; j <= 1; ++j){
				if(x+i<0||x+i>=ht||y+j<0||y+j>=wd||(i==0&&j==0))
					continue;
				/*if(tobj.in[x+i][y+j])
					continue;*/
				//initially the graph has all neighbours
				v1.x=x,v1.y=y;
				v2.x=x+i,v2.y=y+j;
				e.v1=v1,e.v2=v2,e.c=cost(v1,v2);
				hobj.insert(e);
			}
		}
	}

}