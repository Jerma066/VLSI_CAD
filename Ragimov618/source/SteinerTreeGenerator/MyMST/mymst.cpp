#include "mymst.h"

MyMST::MyMST(const QString& file_path):
    xml_fd(XmlReader(file_path)),
    points(xml_fd.getPoints())
{
    std::vector<int> vertical;
    std::vector<int> horizontal;

    for(size_t i = 0; i < points.size(); i++){
        if(find(vertical.begin(), vertical.end(), points[i].x) == vertical.end()){
            vertical.push_back(points[i].x);
        }
        if(find(horizontal.begin(), horizontal.end(), points[i].y) == horizontal.end()){
            horizontal.push_back(points[i].y);
        }
    }

    for(auto j_hor = horizontal.begin(); j_hor != horizontal.end(); j_hor++){
        for(auto i_vert = vertical.begin(); i_vert != vertical.end(); i_vert++){
            candidates.push_back(Candidate(*i_vert, *j_hor));
        }
    }

    std::vector<Point> P_S = points;
    std::vector<Point> T;
    std::vector<Point> S;

    for(auto iter = candidates.begin(); iter != candidates.end(); iter++){
        std::vector<Point> x;
        Point pnt(iter->x, iter->y, Layers::pins);
        x.push_back(pnt);
        if(deltaMST(P_S, x) > 0){
            T.push_back(pnt);
        }
    }

    firstPreparations(P_S, T, S);

    int result;
    Tree Tree_ans;
    tie(result, Tree_ans) = MST(P_S);

    std::vector<int> x;
    std::vector<int> y;
    std::vector<Point> pins;
    for(size_t i = 0; i < points.size(); i++){
        x.push_back(points[i].x);
        y.push_back(points[i].y);
        pins.push_back(points[i]);
    }

    std::vector<Point> pins_m2;
    for(size_t i = 0; i < x.size(); i++){
        pins_m2.push_back(Point(x[i], y[i], Layers::pins_m2));
    }

    points.clear();
    for(auto iter = P_S.begin(); iter != P_S.end(); iter++){
        points.push_back(*iter);
    }

    std::vector<Segment> m2;
    std::vector<Segment> m3;
    std::vector<Segment> final_segments;
    std::vector<TreeElement> m3_pins;
    std::vector<Point> m2_m3;

    for(size_t i = 0; i < Tree_ans.size(); i++){
        if(points[Tree_ans[i].start].x != points[Tree_ans[i].end].x){
            if (points[Tree_ans[i].start].x < points[Tree_ans[i].end].x){
                m2.push_back(Segment(points[Tree_ans[i].start].x, points[Tree_ans[i].start].y,
                                     points[Tree_ans[i].end].x, points[Tree_ans[i].start].y, Layers::m2));
            }
            else{
                m2.push_back(Segment(points[Tree_ans[i].end].x, points[Tree_ans[i].start].y,
                                     points[Tree_ans[i].start].x, points[Tree_ans[i].start].y, Layers::m2));
            }
        }
        if(points[Tree_ans[i].start].y != points[Tree_ans[i].end].y){
            if (points[Tree_ans[i].start].y < points[Tree_ans[i].end].y){
                m3.push_back(Segment(points[Tree_ans[i].end].x, points[Tree_ans[i].start].y,
                                     points[Tree_ans[i].end].x, points[Tree_ans[i].end].y, Layers::m3));
            }
            else{
                m3.push_back(Segment(points[Tree_ans[i].end].x, points[Tree_ans[i].end].y,
                                     points[Tree_ans[i].end].x, points[Tree_ans[i].start].y, Layers::m3));
            }
        }
        if ((points[Tree_ans[i].start].x != points[Tree_ans[i].end].x) &&
            (points[Tree_ans[i].start].y != points[Tree_ans[i].end].y)){
            m3_pins.push_back(TreeElement(0, static_cast<size_t>(points[Tree_ans[i].end].x),
                                             static_cast<size_t>(points[Tree_ans[i].start].y)) );
        }
    }

    for(size_t i = 0; i < m3.size(); i++){
        m3_pins.push_back(TreeElement(0, static_cast<size_t>(m3[i].x1),
                                         static_cast<size_t>(m3[i].y1)) );
        m3_pins.push_back(TreeElement(0, static_cast<size_t>(m3[i].x2),
                                         static_cast<size_t>(m3[i].y2)) );
    }

    std::vector<TreeElement> m3_pins_new;
    for(auto pin = m3_pins.begin(); pin != m3_pins.end(); pin++){
        if(std::find(m3_pins_new.begin(), m3_pins_new.end(), *pin) == m3_pins_new.end()){
            m3_pins_new.push_back(*pin);
        }
    }

    for (auto i = m3_pins_new.begin(); i != m3_pins_new.end(); i++){
        m2_m3.push_back(Point(static_cast<int>(i->start), static_cast<int>(i->end), Layers::m2_m3));
        m2.push_back(Segment(static_cast<int>(i->start), static_cast<int>(i->end),
                             static_cast<int>(i->start), static_cast<int>(i->end), Layers::m2));
    }

    std::sort(m2.begin(), m2.end(), [](const Segment& left, const Segment& right){
        return left.y1 < right.y1;
    });

    std::vector<int> m2_distinct;
    for(auto i = m2.begin(); i != m2.end(); i++){
        if(find(m2_distinct.begin(), m2_distinct.end(), i->y1) == m2_distinct.end()){
            m2_distinct.push_back(i->y1);
        }
    }

    std::vector<Segment> tmp;
    std::vector<std::vector<Segment>> m2s;
    size_t k = 0;

    for(size_t i = 0; i < m2.size(); i++){
        if(m2[i].y1 == m2_distinct[k]){
            tmp.push_back(m2[i]);
        }
        else if(k < m2_distinct.size()){
            m2s.push_back(tmp);
            tmp.clear();
            k += 1;
            tmp.push_back(m2[i]);
        }
    }


    m2s.push_back(tmp);
    tmp.clear();
    std::vector<Segment> final;
    for(size_t i = 0; i < m2s.size(); i++){
        std::sort(m2s[i].begin(), m2s[i].end(), [](const Segment& left, const Segment& right){
            return left.x1 < right.x1;
        });
        for(size_t j = 0; j < m2s[i].size(); j++){
            if(tmp.size() == 0){
                tmp.push_back(m2s[i][j]);
            }
            else{
                if(m2s[i][j].x1 > tmp[tmp.size() - 1].x2){
                    tmp.push_back(m2s[i][j]);
                }
                else{
                    tmp[tmp.size() - 1].x2 =  (m2s[i][j].x2 > tmp[tmp.size() - 1].x2) ? m2s[i][j].x2
                                                                                      : tmp[tmp.size() - 1].x2;
                }
            }
        }
        for (auto k = tmp.begin(); k!= tmp.end(); k++){
            final.push_back(*k);
        }
        tmp.clear();
    }
    for(auto it = final.begin(); it != final.end(); it++){
        final_segments.push_back(*it);
    }

    std::sort(m3.begin(), m3.end(), [](const Segment& left, const Segment& right){
        return left.x1 < right.x1;
    });
    std::vector<int> m3_distinct;
    for(auto i = m3.begin(); i != m3.end(); i++){
        if(find(m3_distinct.begin(), m3_distinct.end(), i->y1) == m3_distinct.end()){
            m3_distinct.push_back(i->x1);
        }
    }

    tmp.clear();
    std::vector<std::vector<Segment>> m3s;
    k=0;

    for(size_t i = 0; i < m3.size(); i++){
        if(m3[i].x1 == m3_distinct[k]){
            tmp.push_back(m3[i]);
        }
        else if(k < m3_distinct.size()){
            m3s.push_back(tmp);
            tmp.clear();
            k += 1;
            tmp.push_back(m3[i]);
        }
    }
    m3s.push_back(tmp);
    tmp.clear();
    final.clear();

    for(size_t i = 0; i < m3s.size(); i++){
        std::sort(m3s[i].begin(), m3s[i].end(), [](const Segment& left, const Segment& right){
            return left.y1 < right.y1;
        });

        for(size_t j = 0; j < m3s[i].size(); j++){
            if(tmp.size() == 0){
                tmp.push_back(m3s[i][j]);
            }
            else{
                if(m3s[i][j].y1 > tmp[tmp.size() - 1].y2){
                    tmp.push_back(m3s[i][j]);
                }
                else{
                    tmp[tmp.size() - 1].y2 =  (m3s[i][j].y2 > tmp[tmp.size() - 1].y2) ? m3s[i][j].y2
                                                                                      : tmp[tmp.size() - 1].y2;
                }
            }
        }
        for (auto k = tmp.begin(); k!= tmp.end(); k++){
            final.push_back(*k);
        }
        tmp.clear();
    }
    //m3 = final;
    for(auto it = final.begin(); it != final.end(); it++){
        final_segments.push_back(*it);
    }
    xml_fd.writeAnswer(pins, final_segments, m2_m3, pins_m2);
}

std::tuple<int, Tree> MyMST::MST(std::vector<Point> P)
{
    //N - вершин
    size_t N = P.size();

    Tree Edges;
    for(size_t i = 0; i < N; i++){
        for(size_t j = 0; j < N; j++){
            if (j > i){
                size_t start = i;
                size_t end = j;

                int weight = abs(P[i].x - P[j].x) + abs(P[i].y - P[j].y);
                Edges.push_back(TreeElement(weight, start, end));
            }
        }
    }

    std::sort(Edges.begin(), Edges.end());

    std::vector<int> comp;
    for(int i = 0; i < static_cast<int>(N); i++){
        comp.push_back(i);
    }

    int min_weight = 0;
    Tree tree;

    for (auto edge = Edges.begin(); edge != Edges.end(); edge++){
        int a = comp[edge->start];
        int b = comp[edge->end];
        if(a != b){
            min_weight += edge->weight;
            tree.push_back(*edge);

            for (size_t i = 0; i < N; i++){
                if (comp[i] == b){
                    comp[i] = a;
                }
            }
        }
    }

    return std::tie(min_weight, tree);
}

int MyMST::deltaMST(std::vector<Point> A, std::vector<Point> B)
{
    std::vector<Point> C;
    for(size_t i = 0; i < A.size(); i++){
        C.push_back(A[i]);
    }
    for(size_t i = 0; i < B.size(); i++){
        C.push_back(B[i]);
    }

    int weight_A, weight_B;
    Tree unused_tree;

    tie(weight_A, unused_tree) = MST(A);
    tie(weight_B, unused_tree) = MST(C);

    return (weight_A - weight_B);
}

void MyMST::firstPreparations(std::vector<Point>& P_S, std::vector<Point>& T, std::vector<Point>& S)
{
    while(T.size() != 0){
        Point max_xx(-1, -1, Layers::pins);
        int max_delta = 0;

        for(auto i = T.begin(); i != T.end(); i++){
            std::vector<Point> x;
            x.push_back(*i);
            int d = deltaMST(P_S, x);
            if(d > max_delta){
                max_delta = d;
                max_xx = *i;
            }
        }

        if(max_xx != Point(-1,-1, Layers::pins)){
            S.push_back(max_xx);
        }

        for(auto it = S.begin(); it != S.end(); it++){
            if(std::find(P_S.begin(), P_S.end(), *it) == P_S.end()){
                P_S.push_back(*it);
            }
        }

        int weight;
        Tree tree;
        std::tie(weight, tree) = MST(P_S);

        for(auto i = S.begin(); i != S.end(); i++){
            int d = 0;
            for(auto t = tree.begin(); t != tree.end(); t++){
                if((P_S[t->start] == *i) || (P_S[t->end] == *i)){
                    d += 1;
                }
            }
        }

        T.clear();
        for(auto i = candidates.begin(); i != candidates.end(); i++){
            std::vector<Point> x;

            Point pnt(i->x, i->y, Layers::pins);
            x.push_back(pnt);

            if(deltaMST(P_S, x) > 0){
                T.push_back(pnt);
            }
        }
    }
}


bool operator==(const TreeElement &lh, const TreeElement &rh)
{
    if((lh.start == rh.start) && (lh.end == rh.end)){
        return true;
    }
    else{
        return false;
    }
}
