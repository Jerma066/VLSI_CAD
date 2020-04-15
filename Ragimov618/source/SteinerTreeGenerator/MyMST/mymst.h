#ifndef MYMST_H
#define MYMST_H

#include <algorithm>

#include "XmlReader/xmlreader.h"

struct Candidate{
    Candidate(const int& x, const int& y):
        x(x), y(y)
    {
    }

    int x;
    int y;
};

struct TreeElement{
    TreeElement(const int& w, const size_t& s, const size_t& e):
        weight(w),
        start(s),
        end(e)
    {
    }

    //Для сортировки
    bool operator<(const TreeElement& right) const {
        return std::tie(weight, start, end) <
               std::tie(right.weight, right.start, right.end);
    }

    int weight;
    size_t start;
    size_t end;
};
bool operator==(const TreeElement& lh ,const TreeElement& rh);

typedef std::vector<TreeElement> Tree;

class MyMST
{
public:
    MyMST(const QString&);

private:
    std::tuple<int, Tree> MST(std::vector<Point>);
    int deltaMST(std::vector<Point>, std::vector<Point>);
    void firstPreparations(std::vector<Point>&, std::vector<Point>&, std::vector<Point>&);
    void createPointsVariable();


private:
    XmlReader xml_fd;
    std::vector<Point> points;
    std::vector<Candidate> candidates;
};

#endif // MYMST_H
