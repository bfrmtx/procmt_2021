#include "gfx_qt_mathplot_set.h"

void ygfxqt::YMathGfxPlotSet2D::createFloatBufferSet() {
    buffer_set = new FloatBufferSet();
    FloatBufferRule rule_x(1, 0, 0, ygfx::shaderlocation_position_x);
    FloatBufferRule rule_y(1, 0, 0, ygfx::shaderlocation_position_y);
    FloatBufferRule rule_add(1, 0, 0, ygfx::shaderlocation_additional_1);
    buffer_set->addFloatBuffer(rule_x, gfx_x_values);
    buffer_set->addFloatBuffer(rule_y, gfx_y_values);
    buffer_set->addFloatBuffer(rule_add, gfx_add_values);
}

ygfxqt::YMathGfxPlotSet2D::~YMathGfxPlotSet2D() {
    if(is_x_data_owner) delete gfx_x_values;
    if(is_y_data_owner) delete gfx_y_values;
    if(is_x_org_data_owner) delete org_x_data;
    if(is_y_org_data_owner) delete org_y_data;
    delete buffer_set;
}

ygfxqt::YMathGfxPlotSet2D::YMathGfxPlotSet2D() :
    is_x_data_owner(true),
    is_y_data_owner(true),
    is_x_org_data_owner(true),
    is_y_org_data_owner(true)
{
    org_x_data = new std::vector<double>();
    org_y_data = new std::vector<double>();
    gfx_x_values = new gfxVector<GLfloat>();
    gfx_y_values = new gfxVector<GLfloat>();
    gfx_add_values = new gfxVector<GLfloat>();
    createFloatBufferSet();
}

ygfxqt::YMathGfxPlotSet2D::YMathGfxPlotSet2D(const QList<QVector2D> & data) :
    is_x_data_owner(false),
    is_y_data_owner(false),
    is_x_org_data_owner(false),
    is_y_org_data_owner(false)
{
    org_x_data = new std::vector<double>();
    org_y_data = new std::vector<double>();

    gfx_x_values = new gfxVector<GLfloat>();
    gfx_y_values = new gfxVector<GLfloat>();
    gfx_add_values = new gfxVector<GLfloat>();
    gfx_x_values->reserve(GLuint(data.size()));
    gfx_y_values->reserve(GLuint(data.size()));

    for(auto const & p : data) {
        org_x_data->push_back(double(p.x()));
        org_y_data->push_back(double(p.y()));
        gfx_x_values->push_back(float(p.x()));
        gfx_y_values->push_back(float(p.y()));
        bounding_box_max.x = std::max(double(p.x()), bounding_box_max.x);
        bounding_box_min.x = std::min(double(p.x()), bounding_box_min.x);
        bounding_box_max.y = std::max(double(p.y()), bounding_box_max.y);
        bounding_box_min.y = std::min(double(p.y()), bounding_box_min.y);
    }
    createFloatBufferSet();
}

ygfxqt::YMathGfxPlotSet2D::YMathGfxPlotSet2D(std::vector<double> * org_x_data, std::vector<double> * org_y_data,
                                             gfxVector<GLfloat> * gfx_x_data, gfxVector<GLfloat> * gfx_y_data,
                                             gfxVector<GLfloat> * gfx_a_data,
                                             spShader shader, QOpenGLTexture * texture) :
    gfx_x_values(gfx_x_data), gfx_y_values(gfx_y_data), gfx_add_values(gfx_a_data),
    org_x_data(org_x_data), org_y_data(org_y_data),
    is_x_data_owner(false),
    is_y_data_owner(false),
    is_x_org_data_owner(false),
    is_y_org_data_owner(false)
{
    setCustomShader(shader);
    setTexture(texture);
    setDrawMode(GL_POINTS);

    createFloatBufferSet();
}

//ygfxqt::YMathGfxPlotSet2D::YMathGfxPlotSet2D(std::vector<double> * org_x_data, std::vector<double> * org_y_data) :
//    is_x_data_owner(true),
//    is_y_data_owner(true),
//    is_x_org_data_owner(false),
//    is_y_org_data_owner(false)
//{
//    gfx_x_values = new gfxVector<GLfloat>();
//    gfx_y_values = new gfxVector<GLfloat>();
//    gfx_add_values = new gfxVector<GLfloat>();
//    this->org_x_data = org_x_data;
//    this->org_y_data = org_y_data;
//    for(double x : * org_x_data) {
//        gfx_x_values->push_back(static_cast<GLfloat>(x));
//        bounding_box_max.x = std::max(x, bounding_box_max.x);
//        bounding_box_min.x = std::min(x, bounding_box_min.x);
//    }
//    for(double y : *org_y_data) {
//        gfx_y_values->push_back(static_cast<GLfloat>(y));
//        bounding_box_max.y = std::max(y, bounding_box_max.y);
//        bounding_box_min.y = std::min(y, bounding_box_min.y);
//    }

//    createFloatBufferSet();
//}

void ygfxqt::YMathGfxPlotSet2D::appendData(const std::vector<double> & xData, const std::vector<double> & yData) {
    org_x_data->insert(org_x_data->end(), xData.begin(), xData.end());
    org_y_data->insert(org_y_data->end(), yData.begin(), yData.end());
    for(auto x : xData) {
        gfx_x_values->push_back(static_cast<GLfloat>(x));
        bounding_box_max.x = std::max(x, bounding_box_max.x);
        bounding_box_min.x = std::min(x, bounding_box_min.x);
    }
    for(auto y : yData) {
        gfx_y_values->push_back(static_cast<GLfloat>(y));        
        bounding_box_max.y = std::max(y, bounding_box_max.y);
        bounding_box_min.y = std::min(y, bounding_box_min.y);
    }
    for(size_t i = 0; i < std::max(xData.size(), yData.size()); ++i) {
        gfx_add_values->push_back(0.0f);
    }
}

void ygfxqt::YMathGfxPlotSet2D::appendData(double x, double y) {
    org_x_data->push_back(x);
    org_y_data->push_back(y);
    gfx_x_values->push_back(static_cast<GLfloat>(x));
    gfx_y_values->push_back(static_cast<GLfloat>(y));
    bounding_box_max.x = std::max(x, bounding_box_max.x);
    bounding_box_min.x = std::min(x, bounding_box_min.x);
    bounding_box_max.y = std::max(y, bounding_box_max.y);
    bounding_box_min.y = std::min(y, bounding_box_min.y);
    gfx_add_values->push_back(0.0f);
}

void ygfxqt::YMathGfxPlotSet2D::setDataX(size_t index, double value) {
    if(index < org_x_data->size() && GLint(index) < gfx_x_values->size()) {
        org_x_data->at(index) = value;
        (*gfx_x_values)[GLuint(index)] = static_cast<GLfloat>(value);
    }
}

void ygfxqt::YMathGfxPlotSet2D::setDataY(size_t index, double value) {    
    if(index < org_y_data->size() && GLint(index) < gfx_y_values->size()) {
        org_y_data->at(index) = value;
        (*gfx_y_values)[GLuint(index)] = static_cast<GLfloat>(value);
        bounding_box_max.y = std::max(value, bounding_box_max.y);
        bounding_box_min.y = std::min(value, bounding_box_min.y);
    }
}

void ygfxqt::YMathGfxPlotSet2D::setData(size_t index, double x, double y) {
    setDataX(index, x);
    setDataY(index, y);
}

void ygfxqt::YMathGfxPlotSet2D::setDataA(size_t index, float value) {
    if(gfx_add_values->size() > GLint(index)) {
        (*gfx_add_values)[GLuint(index)] = value;
    }
}

void ygfxqt::YMathGfxPlotSet2D::clearData() {
    org_x_data->clear();
    org_y_data->clear();
    gfx_x_values->clear();
    gfx_y_values->clear();
    bounding_box_max = glm::dvec2(10, 10);
    bounding_box_min = glm::dvec2(-10, -10);
}

void ygfxqt::YMathGfxPlotSet2D::removeLastData(size_t size) {
    if(org_x_data->size() > size) org_x_data->resize(org_x_data->size() - size); else org_x_data->clear();
    if(org_y_data->size() > size) org_y_data->resize(org_y_data->size() - size); else org_y_data->clear();
    if(gfx_x_values->size() > GLint(size)) gfx_x_values->resize(GLuint(gfx_x_values->size()) - GLuint(size)); else gfx_x_values->clear();
    if(gfx_y_values->size() > GLint(size)) gfx_y_values->resize(GLuint(gfx_y_values->size()) - GLuint(size)); else gfx_y_values->clear();
}

void ygfxqt::YMathGfxPlotSet2D::draw() {
    GLsizei cSize = std::min(static_cast<GLsizei>(gfx_x_values->size()), draw_size);
    GLint cOffset = draw_offset;

    buffer_set->draw(this->drawmode, cOffset, cSize);
}
