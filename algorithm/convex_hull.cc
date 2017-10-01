#include <vector>
#include <algorithm>
#include <cstdio>
using namespace std;

struct Point {
    int x, y;
    Point(): x(0), y(0) {}
    Point(int a, int b): x(a), y(b) {}
};
bool cmp(const Point& l, const Point& r) {
    if (l.x != r.x) return l.x < r.x;
    return l.y < r.y;
}
bool eq(const Point& l, const Point& r) {
    return l.x == r.x && l.y == r.y;
}
int det(const Point& l, const Point& m, const Point& r) {
    return (m.x - l.x) * (r.y - m.y) - (r.x - m.x) * (m.y - l.y);
}
vector<Point> convex_hull(vector<Point>& points) {
    sort(begin(points), end(points), cmp);
    // remove duplicated points
    points.erase(unique(begin(points), end(points), eq), end(points));
    int n = points.size(), k = 0;
    vector<Point> ans(2*n);
    // lower edge
    for (int i=0; i<n; i++) {
        // use <= 0 will remove those points at the same line
        while(k > 1 && det(ans[k-2], ans[k-1], points[i]) < 0) k--;
        ans[k++] = points[i];
    }
    // upper edge
    for (int i=n-2, t=k; i>=0; i--) {
        while(k > t && det(ans[k-2], ans[k-1], points[i]) < 0) k--;
        ans[k++] = points[i];
    }
    ans.resize(k);
    // assert(ans.front() == ans.back());
    // the first point would be duplicated on the returned path.
    return ans;
}

int main() {
    vector<Point> pts {{1,1}, {2,2}, {2,0}, {2,4}, {3,3}, {4,2}}, ans;
    ans = convex_hull(pts);
    for (auto& pt : ans) {
        printf("{%d,%d}, ", pt.x, pt.y);
    }
    return 0;
}
