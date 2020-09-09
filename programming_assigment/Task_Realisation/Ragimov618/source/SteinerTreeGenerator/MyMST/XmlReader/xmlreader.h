#ifndef XMLREADER_H
#define XMLREADER_H

#include <QObject>

#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>
#include <QFileInfo>

#include <map>
#include <vector>
#include <tuple>

enum class Layers
{
    pins,
    m2,
    m3,
    m2_m3,
    pins_m2
};

struct Point{
    Point()
    {
    }

    Point(const int& x, const int& y, const Layers& layer):
        x(x),
        y(y),
        layer(layer)
    {
    }

    int x;
    int y;
    Layers layer;
};
bool operator==(const Point& lh, const Point& rh);
bool operator!=(const Point& lh, const Point& rh);

struct Segment{
    Segment(){
    }

    Segment(const int& x1, const int& y1,const int& x2,
            const int& y2, const Layers& layer):
        x1(x1), x2(x2),
        y1(y1), y2(y2),
        layer(layer)
    {
    }

    int x1;
    int x2;
    int y1;
    int y2;
    Layers layer;
};

class XmlReader
{
public:
    explicit XmlReader(const QString&);
    std::tuple<int, int, int, int> getMinMaxValues() const;
    std::vector<Point> getPoints() const;
    std::vector<Segment> getSegments() const;

    void writeAnswer(std::vector<Point> pins,
                     std::vector<Segment> final_segments,
                     std::vector<Point> m2_m3,
                     std::vector<Point> pins_m2);

private:
    QString _fpath;
    QString _w_fpath;
    int min_x, max_x, min_y, max_y;
    std::vector<Point> points;
    std::vector<Segment> segments;
    std::map<Layers, QString> dict_of_names;
};

#endif // XMLREADER_H
