#include "xmlreader.h"

XmlReader::XmlReader(const QString& file_path):
    _fpath(file_path)
{
    std::map<QString, Layers> l_nicks = {
        {"pins", Layers::pins},
        {"m2", Layers::m2},
        {"m3", Layers::m3},
        {"m2_m3", Layers::m2_m3},
        {"pins_m2", Layers::pins_m2}
    };

    dict_of_names.insert({Layers::pins, "pins"});
    dict_of_names.insert({Layers::m2, "m2"});
    dict_of_names.insert({Layers::m3, "m3"});
    dict_of_names.insert({Layers::m2_m3, "m2_m3"});
    dict_of_names.insert({Layers::pins_m2, "pins_m2"});

    QFile file(_fpath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QString err = "File Error: Can not open file!";
        qDebug() << err;
    }
    else {
        QXmlStreamReader xmlReader;
        xmlReader.setDevice(&file);
        xmlReader.readNext();

        while(!xmlReader.atEnd())
        {
            if(xmlReader.isStartElement())
            {
                if(xmlReader.name() == "grid"){
                    foreach(const QXmlStreamAttribute &attr, xmlReader.attributes()){
                        if(attr.name().toString() == "min_x"){
                            min_x = attr.value().toInt();
                        }
                        else if(attr.name().toString() == "max_x"){
                            max_x = attr.value().toInt();
                        }
                        else if(attr.name().toString() == "min_y"){
                            min_y = attr.value().toInt();
                        }
                        else if(attr.name().toString() == "max_y"){
                            max_y = attr.value().toInt();
                        }
                    }
                }

                if(xmlReader.name() == "point"){
                    Point element;
                    foreach(const QXmlStreamAttribute &attr, xmlReader.attributes()){
                        if(attr.name().toString() == "x"){
                            element.x = attr.value().toInt();
                        }
                        else if(attr.name().toString() == "y"){
                            element.y = attr.value().toInt();
                        }
                        else if(attr.name().toString() == "layer"){
                            element.layer = l_nicks[attr.value().toString()];
                        }
                    }
                    points.push_back(element);
                }

                if(xmlReader.name() == "segment"){
                    Segment element;
                    foreach(const QXmlStreamAttribute &attr, xmlReader.attributes()){
                        if(attr.name().toString() == "x1"){
                            element.x1 = attr.value().toInt();
                        }
                        else if(attr.name().toString() == "x2"){
                            element.x2 = attr.value().toInt();
                        }
                        else if(attr.name().toString() == "y1"){
                            element.y1 = attr.value().toInt();
                        }
                        else if(attr.name().toString() == "y2"){
                            element.y2 = attr.value().toInt();
                        }
                        else if(attr.name().toString() == "layer"){
                            element.layer = l_nicks[attr.value().toString()];
                        }
                    }
                    segments.push_back(element);
                }
            }
            xmlReader.readNext();
        }
    }
    file.close();
}

std::tuple<int, int, int, int> XmlReader::getMinMaxValues() const
{
    return std::tie(min_x, max_x, min_y, max_y);
}

std::vector<Point> XmlReader::getPoints() const
{
    return points;
}

std::vector<Segment> XmlReader::getSegments() const
{
    return segments;
}

void XmlReader::writeAnswer(std::vector<Point> pins,
                            std::vector<Segment> final_segments,
                            std::vector<Point> m2_m3,
                            std::vector<Point> pins_m2)
{
    QFileInfo file_info(_fpath);
    _w_fpath = file_info.path();
    _w_fpath += "/" + file_info.baseName() + "_out." + file_info.suffix();

    QFile file(_w_fpath);
    file.open(QIODevice::WriteOnly);
    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement("root");

    xmlWriter.writeStartElement("grid");
    xmlWriter.writeAttribute("min_x", QString::number(min_x));
    xmlWriter.writeAttribute("max_x", QString::number(max_x));
    xmlWriter.writeAttribute("min_y", QString::number(min_y));
    xmlWriter.writeAttribute("max_y", QString::number(max_y));
    xmlWriter.writeEndElement(); // </grid>

    xmlWriter.writeStartElement("net");

    for(size_t i = 0; i < pins.size(); i++){
        xmlWriter.writeStartElement("point");
        xmlWriter.writeAttribute("x", QString::number(pins[i].x));
        xmlWriter.writeAttribute("y", QString::number(pins[i].y));
        xmlWriter.writeAttribute("layer", "pins");
        xmlWriter.writeEndElement(); //</point>
    }

    for(size_t i = 0; i < m2_m3.size(); i++){
        xmlWriter.writeStartElement("point");
        xmlWriter.writeAttribute("x", QString::number(m2_m3[i].x));
        xmlWriter.writeAttribute("y", QString::number(m2_m3[i].y));
        xmlWriter.writeAttribute("layer", "m2_m3");
        xmlWriter.writeEndElement(); // </point>
    }

    for(size_t i = 0; i < pins_m2.size(); i++){
        xmlWriter.writeStartElement("point");
        xmlWriter.writeAttribute("x", QString::number(pins_m2[i].x));
        xmlWriter.writeAttribute("y", QString::number(pins_m2[i].y));
        xmlWriter.writeAttribute("layer", "pins_m2");
        xmlWriter.writeEndElement(); // </point>
    }

    for(size_t i = 0; i < final_segments.size(); i++){
        if(final_segments[i].x1 == final_segments[i].x2 &&
           final_segments[i].x1 == final_segments[i].y2){
            xmlWriter.writeStartElement("point");
            xmlWriter.writeAttribute("x", QString::number(final_segments[i].x1));
            xmlWriter.writeAttribute("y", QString::number(final_segments[i].y1));
            xmlWriter.writeAttribute("layer", dict_of_names[final_segments[i].layer]);
            xmlWriter.writeEndElement(); // </point>
        }
    }
    for(size_t i = 0; i < final_segments.size(); i++){
        if(!(final_segments[i].x1 == final_segments[i].x2 &&
           final_segments[i].x1 == final_segments[i].y2)){
            xmlWriter.writeStartElement("segment");
            xmlWriter.writeAttribute("x1", QString::number(final_segments[i].x1));
            xmlWriter.writeAttribute("y1", QString::number(final_segments[i].y1));
            xmlWriter.writeAttribute("x2", QString::number(final_segments[i].x2));
            xmlWriter.writeAttribute("y2", QString::number(final_segments[i].y2));
            xmlWriter.writeAttribute("layer", dict_of_names[final_segments[i].layer]);
            xmlWriter.writeEndElement(); //</segment>
        }
    }

    /*
    for(size_t i = 0; i < m2.size(); i++){
        xmlWriter.writeStartElement("segment");
        xmlWriter.writeAttribute("x1", QString::number(m2[i].x1));
        xmlWriter.writeAttribute("y1", QString::number(m2[i].y1));
        xmlWriter.writeAttribute("x2", QString::number(m2[i].x2));
        xmlWriter.writeAttribute("y2", QString::number(m2[i].y2));
        xmlWriter.writeAttribute("layer", "m2");
        xmlWriter.writeEndElement(); //</segment>
    }

    for(size_t i = 0; i < m3.size(); i++){
        xmlWriter.writeStartElement("segment");
        xmlWriter.writeAttribute("x1", QString::number(m3[i].x1));
        xmlWriter.writeAttribute("y1", QString::number(m3[i].y1));
        xmlWriter.writeAttribute("x2", QString::number(m3[i].x2));
        xmlWriter.writeAttribute("y2", QString::number(m3[i].y2));
        xmlWriter.writeAttribute("layer", "m3");
        xmlWriter.writeEndElement(); //</segment>
    }
    */

    xmlWriter.writeEndElement(); // </net>

    xmlWriter.writeEndElement(); // </root>

    xmlWriter.writeEndDocument();
    file.close();

    qDebug() << "Complete!";
}

bool operator==(const Point &lh, const Point &rh){
    if( (lh.x == rh.x) &&
        (lh.y == rh.y) &&
        (lh.layer == rh.layer))
    {
        return true;
    }
    else{
        return false;
    }
}

bool operator!=(const Point& lh, const Point& rh){
    return !(lh == rh);
}
