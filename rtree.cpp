#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <fstream>
using namespace std;

class Point{
public:
    int x,y;
    Point(int x,int y){
        this->x=x;
        this->y=y;
    }
};

int maxPerimetro(vector<Point> points){
    int maxx=points[0].x,minx=maxx,maxy=points[0].y,miny=maxy;
    for(int i=1; i<points.size(); i++){
        if(points[i].x > maxx)  maxx=points[i].x;
        if(points[i].x < minx)  minx=points[i].x;
        if(points[i].y > maxy)  maxy=points[i].y;
        if(points[i].y < miny)  miny=points[i].y;
    }
    return 2*((maxx-minx)+(maxy-miny));
}
bool sortbyx(Point a,Point b){
    return (a.x < b.x);
}
bool sortbyy(Point a, Point b){
    return (a.y < b.y);
}
vector<Point> crear(vector<Point> points){
    int maxx=points[0].x,minx=points[0].x,maxy=points[0].y,miny=points[0].y;
    for(int i=0; i<points.size(); i++){
        if(points[i].x > maxx)  maxx=points[i].x;
        if(points[i].x < minx)  minx=points[i].x;
        if(points[i].y > maxy)  maxy=points[i].y;
        if(points[i].y < miny)  miny=points[i].y;
    }
    Point v1(minx,miny);
    Point v2(minx,maxy);
    Point v3(maxx,miny);
    Point v4(maxx,maxy);
    return (vector<Point> {v1,v2,v3,v4});
}

class Node;
class MBR{
public:
    vector<Point> vertices;
    Node* child;
public:
    MBR(vector<Point> points,Node* child){
        this->vertices=crear(points);
        this->child=child;
    }
    bool contiene(Point p){
        if(vertices[0].x <= p.x &&
            vertices[0].y <= p.y &&
            vertices[3].x >= p.x &&
            vertices[3].y >= p.y)
            return true;
        else
            return false;
    }
    int crece(Point p){
        int newx,newy,x,y;
        newx=min(abs(vertices[0].x - p.x),abs(vertices[3].x - p.x));
        newy=min(abs(vertices[0].y - p.y),abs(vertices[3].y - p.y));
        return newx+newy;
    }
    bool interseca(MBR u){
        for(int i=0; i<u.vertices.size(); i++){
            if(this->contiene(u.vertices[i]))
                return true;
        }
        return false;
    }
};
bool sortbyleftx(MBR a,MBR b){
    return (a.vertices[0].x < b.vertices[0].x);
}
bool sortbylefty(MBR a,MBR b){
    return (a.vertices[0].y < b.vertices[0].y);
}
bool sortbyrightx(MBR a,MBR b){
    return (a.vertices[4].x < b.vertices[4].x);
}
bool sortbyrighty(MBR a,MBR b){
    return (a.vertices[4].y < b.vertices[4].y);
}
int maxPerimetro(vector<MBR> mbr){
    int maxx=mbr[0].vertices[0].x,minx=maxx;
    int maxy=mbr[0].vertices[0].y,miny=maxy;
    for(int i=0; i<mbr.size(); i++){
        if(mbr[i].vertices[3].x > maxx)  maxx=mbr[i].vertices[3].x;
        if(mbr[i].vertices[0].x < minx)  minx=mbr[i].vertices[0].x;
        if(mbr[i].vertices[3].y > maxy)  maxy=mbr[i].vertices[3].y;
        if(mbr[i].vertices[0].y < miny)  miny=mbr[i].vertices[0].y;
    }
    return 2*((maxx-minx)+(maxy-miny));
}
class Node{
public:
    vector<Point> points;
    vector<MBR> mbrs;
    bool is_leaf;
    int deep;
    Node* parent;
    MBR* MBRparent;
    //debemos guardar la direccion del bmr parent
    //para luego poder actualizarlo cuando hagamos un split
    //en el overñaklsjdfñl
public:
    Node(int deep){
        this->is_leaf=true;
        this->deep=deep;
    }
};
class Rtree{
public:
    Node* root;
    int capacidad;
public:
    Rtree(int capacidad){
        this->root=new Node(0);
        this->capacidad=capacidad;
    }
    Node* chooseSubtree(Node* node,Point p){
        int minPerimetro=node->mbrs[0].crece(p);
        Node* result=node->mbrs[0].child;
        for(int i=0; i<node->mbrs.size(); i++){
            if(node->mbrs[i].contiene(p))
                return node->mbrs[i].child;
            else{
                int temp=node->mbrs[i].crece(p);
                if(temp < minPerimetro){
                    minPerimetro=temp;
                    result=node->mbrs[i].child;
                }
            }
        }
        return result;
    }
    //algo falla
    void actualizarMBR(Node* node){
        if(node == this->root)
            return;
        if(node->is_leaf){
            for(int i=0; i<node->parent->mbrs.size(); i++){
                if((node->parent->mbrs[i].child) == node){
                    node->parent->mbrs[i].vertices=crear(node->points);
                    break;
                }
            }
            actualizarMBR(node->parent);
        }else{
            for(int i=0; i<node->parent->mbrs.size(); i++){
                if((node->parent->mbrs[i].child) == node){
                    vector<Point> p;
                    for(int j=0; j<node->mbrs.size(); j++)
                        copy(node->mbrs[j].vertices.begin(),node->mbrs[j].vertices.end(),back_inserter(p));
                    node->parent->mbrs[i].vertices=crear(p);
                    break;
                }
            }
            actualizarMBR(node->parent);
        }
    }
    Node* splitMBR(Node*& node){
        int m = node->mbrs.size();
        int limit = ceil(0.4*capacidad);
        int bestPerimetro = 999999999;
        vector<MBR> bestMBR1,bestMBR2;
        for(int axis=0; axis<2; axis++){
            for(int direc=0; direc<2; direc++){
                if(axis==0 && direc==0)
                    sort(node->mbrs.begin(),node->mbrs.end(),sortbyleftx);
                if(axis==0 && direc==1)
                    sort(node->mbrs.begin(),node->mbrs.end(),sortbyrightx);
                if(axis==1 && direc==0)
                    sort(node->mbrs.begin(),node->mbrs.end(),sortbylefty);
                if(axis==1 && direc==1)
                    sort(node->mbrs.begin(),node->mbrs.end(),sortbyrighty);
                for(int i=limit; i<=(m-limit); i++){
                    vector<MBR> S1,S2;
                    for(int j=0; j<node->mbrs.size(); j++){
                        if(j<i) S1.push_back(node->mbrs[j]);
                        else    S2.push_back(node->mbrs[j]);
                    }
                    int temPerimetro=maxPerimetro(S1) + maxPerimetro(S2);
                    if(bestPerimetro > temPerimetro){
                        bestPerimetro = temPerimetro;
                        bestMBR1 = S1;
                        bestMBR2 = S2;
                    }
                }
            }
        }
        Node* newNode=new Node(node->deep);
        newNode->is_leaf=false;
        newNode->mbrs = bestMBR2;
        node->mbrs = bestMBR1;
        return newNode;
    }
    Node* split(Node*& node){
        if(!node->is_leaf)
            return splitMBR(node);
        int m=node->points.size();
        int limit=ceil(0.4*capacidad);
        int bestPerimetro=maxPerimetro(node->points)*5;
        vector<Point> bestMBR1,bestMBR2;
        for(int axis=0; axis<2; axis++){
            if(axis == 0)
                sort(node->points.begin(),node->points.end(),sortbyx);
            else
                sort(node->points.begin(),node->points.end(),sortbyy);
            for(int i=limit; i<=(m-limit); i++){
                vector<Point> S1,S2;
                for(int j=0; j<node->points.size(); j++){
                    if(j<i) S1.push_back(node->points[j]);
                    else    S2.push_back(node->points[j]);
                }
                int temPerimetro=maxPerimetro(S1) + maxPerimetro(S2);
                if(bestPerimetro > temPerimetro){
                    bestPerimetro = temPerimetro;
                    bestMBR1 = S1;
                    bestMBR2 = S2;
                }
            }
        }
        Node* newNode = new Node(node->deep);
        newNode->points = bestMBR2;
        node->points = bestMBR1;
        return newNode;
    }
    void actualizarDeep(Node* node,int deep){
        node->deep=deep;
        if(node->is_leaf)   return;
        for(int i=0; i<node->mbrs.size(); i++){
            actualizarDeep(node->mbrs[i].child,deep+1);
        }
    }
    void overflow(Node* node){
        Node* node2 = split(node);
        if(node == this->root){
            Node* newRoot = new Node(0);
            newRoot->is_leaf=false;
            node->parent=newRoot;
            node2->parent=newRoot;
            vector<Point> vec1,vec2;
            if(node->is_leaf){
                vec1=node->points;
                vec2=node2->points;
            }else{
                for(int i=0; i<node->mbrs.size(); i++)
                    copy(node->mbrs[i].vertices.begin(), node->mbrs[i].vertices.end(), back_inserter(vec1));
                for(int i=0; i<node2->mbrs.size(); i++)
                    copy(node2->mbrs[i].vertices.begin(), node2->mbrs[i].vertices.end(), back_inserter(vec2));
            }
            MBR mbr1(vec1,node);
            MBR mbr2(vec2,node2);
            newRoot->mbrs.push_back(mbr1);
            //node->MBRparent=&(newRoot->mbrs[0]);
            newRoot->mbrs.push_back(mbr2);
            //node2->MBRparent = &(newRoot->mbrs[1]);
            actualizarDeep(newRoot,0);
            this->root=newRoot;
        }else{
            Node* padre=node->parent;
            vector<Point> vert1,vert2;
            if(node->is_leaf){
                vert1=node->points;
                vert2 = node2->points;
            }
            else{
                for(int i=0; i<node->mbrs.size(); i++)
                    copy(node->mbrs[i].vertices.begin(), node->mbrs[i].vertices.end(), back_inserter(vert1));
                for(int i=0; i<node2->mbrs.size(); i++){
                    copy(node2->mbrs[i].vertices.begin(), node2->mbrs[i].vertices.end(), back_inserter(vert2));
                }
            }
            MBR mbr1(vert1,node);
            MBR mbr2(vert2,node2);
            //MBR* mbr_ = node->MBRparent;//actualizaremos el mbr del primer nodo, ya existia
            //*mbr_ = mbr1;
            padre->mbrs.push_back(mbr2);
            int pos = padre->mbrs.size()-1;
            node2->parent = node->parent;
            //node2->MBRparent = &(padre->mbrs[pos]);//damos la direccion del bmr que contiene a node2
            if(padre->mbrs.size() > this->capacidad){
                overflow(padre);
            }
            actualizarMBR(node);
            //actualizarMBR(node2);
        }
    }
    void insert(Node* node,Point p){
        if(node->is_leaf){
            node->points.push_back(p);
            if(node->points.size() > this->capacidad){
                overflow(node);
            }
            actualizarMBR(node);
        }else{
            Node* newNode=chooseSubtree(node,p);
            insert(newNode,p);
        }
    }
    void insert(Point p){
        return insert(this->root,p);
    }
    void print(Node* node){
        if(node->is_leaf){
            cout<<"nivel hoja "<<node->deep<<": ";
            for(int i=0; i<node->points.size(); i++)
                cout<<node->points[i].x<<","<<node->points[i].y<<" ";
            cout<<endl;
        }else{
            cout<<"nivel "<<node->deep<<endl;
            for(int i=0; i<node->mbrs.size(); i++){
                cout<<"MBR: ";
                for(int j=0; j<4; j++)
                    cout<<node->mbrs[i].vertices[j].x<<"/"<<node->mbrs[i].vertices[j].y<<endl;
                print(node->mbrs[i].child);
            }
        }
    }
    void graph_py(Node* node,ostream &mbr,ostream &point){
        if(node->is_leaf){
            for(int i=0; i<node->points.size(); i++)
                point<<node->points[i].x<<","<<node->points[i].y<<endl;
        }else{
            for(int i=0; i<node->mbrs.size(); i++){
                for(int j=0; j<4; j++)
                    mbr<<node->mbrs[i].vertices[j].x<<","<<node->mbrs[i].vertices[j].y<<endl;
                graph_py(node->mbrs[i].child,mbr,point);
            }
        }
    }
    void graph_py(ostream &mbr,ostream &points){
        return graph_py(this->root,mbr,points);
    }
    void print_dot(Node* node,ostream &os){
        if(node->is_leaf){
            cout<<"dot leaf"<<endl;
            int x1=node->points[0].x;
            int y1=node->points[0].y;
            os<<"parent"<<x1<<y1<<node->deep<<"[\n"<<"shape=plaintext"<<endl;
            os<<"label=<\n"<<"<table border='1' cellborder='1'>"<<endl;
            os<<"<tr>";
            for(int i=0; i<node->points.size(); i++){
                int x=node->points[i].x;
                int y=node->points[i].y;
                os<<"<td port='port"<<x<<y<<"'>\""<<x<<","<<y<<"\"</td>";
            }
            os<<"</tr></table>\n"<<">];"<<endl;
        }else{
            cout<<"dot mbr"<<endl;
            int x1=node->mbrs[0].vertices[0].x;
            int y1=node->mbrs[0].vertices[0].y;
            os<<"parent"<<x1<<y1<<node->deep<<"[\n"<<"shape=plaintext"<<endl;
            os<<"label=<\n"<<"<table border='1' cellborder='1'>"<<endl;
            os<<"<tr>";
            for(int i=0; i<node->mbrs.size(); i++){
                int x=node->mbrs[i].vertices[0].x;
                int y=node->mbrs[i].vertices[0].y;
                os<<"<td port='port"<<x<<y<<"'>"<<"\"MBR: "<<x<<","<<y<<"\"</td>";
            }
            os<<"</tr></table>\n"<<">];"<<endl;
            for(int i=0; i<node->mbrs.size(); i++){
                int x=node->mbrs[i].vertices[0].x;
                int y=node->mbrs[i].vertices[0].y;
                print_dot(node->mbrs[i].child,os);
                os<<"parent"<<x1<<y1<<node->deep<<":port"<<x<<y<<"->";
                if(node->mbrs[0].child->is_leaf == true){
                    int x = node->mbrs[i].child->points[0].x;
                    int y = node->mbrs[i].child->points[0].y;
                    os<<"parent"<<x<<y<<node->deep + 1<<";"<<endl;
                }else{
                    int x = node->mbrs[i].child->mbrs[0].vertices[0].x;
                    int y = node->mbrs[i].child->mbrs[0].vertices[0].y;
                    os<<"parent"<<x<<y<<node->deep + 1<<";"<<endl;
                }
            }
        }
    }
    void print_dot(ostream &os){
        os<<"digraph H {"<<endl;
        print_dot(this->root,os);
        os<<"}"<<endl;
    }
    void print(){
        print(this->root);
    }
    void rangeQuery(Node* node,MBR range,vector<Point> &result){
        if(node->is_leaf){
            cout<<" searh leaf"<<endl;
            for(int i=0; i<node->points.size(); i++){
                cout<<"comparando: "<<node->points[i].x<<node->points[i].y<<endl;
                if(range.contiene(node->points[i]))
                    result.push_back(node->points[i]);
            }
        }else{
            cout<<" searh internal"<<endl;
            for(int i=0; i<node->mbrs.size(); i++){
                if(node->mbrs[i].interseca(range) || range.interseca(node->mbrs[i]))
                    rangeQuery(node->mbrs[i].child,range,result);
            }
        }
    }
    vector<Point> rangeQuery(int h,int w,Point center){
        int minx = center.x - (w/2);
        int maxx = center.x + (w/2);
        int miny = center.y - (h/2);
        int maxy = center.y + (h/2);
        Point v1(minx,miny);
        Point v2(minx,maxy);
        Point v3(maxx,miny);
        Point v4(maxx,maxy);
        MBR range(vector<Point> {v1,v2,v3,v4},NULL);
        cout<<range.vertices[0].x<<","<<range.vertices[0].y<<" ";
        cout<<range.vertices[3].x<<","<<range.vertices[3].y<<endl;
        vector<Point> result;
        rangeQuery(this->root,range,result);
        return result;
    }
};
int main(){
    Point p1(2,5);
    Point p2(5,4);
    Point p3(1,2);
    Point p4(3,3);
    vector<Point> verti{p1,p2,p3};
    MBR recta(verti,NULL);
    Rtree tree(3);
    tree.insert(Point(1,5));
    tree.insert(Point(4,1));
    tree.insert(Point(3,14));
    tree.insert(Point(4,10));
    tree.insert(Point(5,7));
    tree.insert(Point(8,13));
    tree.insert(Point(9,9));
    tree.insert(Point(11,11));
    tree.insert(Point(13,12));
    tree.insert(Point(10,6));
    tree.insert(Point(15,8));
    tree.insert(Point(14,4));
    tree.insert(Point(13,2));
    tree.print();
    cout<<recta.contiene(Point(5,6))<<endl;
    vector<Point> range=tree.rangeQuery(2,2,Point(3,10));
    cout<<range.size()<<endl;
    for(int i=0; i<range.size(); i++){
        cout<<range[i].x<<","<<range[i].y<<" | ";
    }
    ofstream F("graph.dot");
    tree.print_dot(F);
    ofstream M("mbr.txt");
    ofstream P("point.txt");
    tree.graph_py(M,P);
    system("python3 graph.py");
    return 0;
}
