#include <bits/stdc++.h>

struct Vector{
	Vector(){
	
	}
	Vector(int _x, int _y){
		x = _x;
		y = _y;
	}	
	int x, y;
};

bool cmpX(Vector a, Vector b){
	return a.x < b.x;
}

bool cmpY(Vector a, Vector b){
	return a.y < b.y;
}

struct KDTreeNode{
	Vector pos;
	bool isLeaf;

	int x0 = std::numeric_limits<int>::max(), y0 = std::numeric_limits<int>::max();
	int x1 = -std::numeric_limits<int>::max(), y1 = -std::numeric_limits<int>::max();

	KDTreeNode* lson;
	KDTreeNode* rson;

	int distance(const Vector& _vec){ // 求一点到这个结点中所有点的包围盒的最小距离
		int x = (_vec.x < x0 ? x0 : _vec.x > x1 ? x1 : _vec.x);
		int y = (_vec.y < y0 ? y0 : _vec.y > y1 ? y1 : _vec.y);
		return (x - _vec.x) * (x - _vec.x) + (y - _vec.y) * (y - _vec.y);
	}

	KDTreeNode(std::vector<Vector>&& _vec){
		if(_vec.size() == 1){
			isLeaf = true;
			pos = _vec[0];
		}else{
			for(auto p : _vec){
				x0 = std::min(x0, p.x), y0 = std::min(y0, p.y);
				x1 = std::max(x1, p.x), y1 = std::max(y1, p.y);
			}
			std::sort(_vec.begin(), _vec.end(), x1 - x0 > y1 - y0 ? cmpX : cmpY);

			int mid = _vec.size() >> 1;
			lson = new KDTreeNode({_vec.begin(), _vec.begin() + mid});
			rson = new KDTreeNode({_vec.begin() + mid, _vec.end()});
		}
	}
};

struct KDTree{
	KDTreeNode* root;

	KDTree(const std::vector<Vector>& _vec) : root(new KDTreeNode({_vec.begin(), _vec.end()})) {}
};

std::vector<Vector> p; // 所有的点

int main(){
	return 0;
}
