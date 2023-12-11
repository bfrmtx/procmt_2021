#ifndef GFX_QT_MATHPLOT_SET_H
#define GFX_QT_MATHPLOT_SET_H

#include <QVector2D>

#include "ygfx/gfxconstexpr.h"
#include "ygfx/gfxvector.h"
#include "ygfx/gfxfloatbufferset.h"

#include "gfx_qt_mathplot.h"


namespace ygfxqt {
    class YMathGfxPlotSet2D : public YMathGfxPlot {
    private:
        gfxVector<GLfloat> * gfx_x_values = nullptr;
        gfxVector<GLfloat> * gfx_y_values = nullptr;
        gfxVector<GLfloat> * gfx_add_values = nullptr;
        std::vector<double> * org_x_data = nullptr;
        std::vector<double> * org_y_data = nullptr;
        FloatBufferSet * buffer_set = nullptr;
        bool is_x_data_owner = false;
        bool is_y_data_owner = false;
        bool is_x_org_data_owner = false;
        bool is_y_org_data_owner = false;

    protected:
        void createFloatBufferSet();

    public:
        virtual ~YMathGfxPlotSet2D() override;
        virtual void draw() override;

        explicit YMathGfxPlotSet2D();
        explicit YMathGfxPlotSet2D(QList<QVector2D> const & data);
        explicit YMathGfxPlotSet2D(std::vector<double> * org_x_data, std::vector<double> * org_y_data,
                                   gfxVector<GLfloat> * gfx_x_data, gfxVector<GLfloat> * gfx_y_data, gfxVector<GLfloat> * gfx_a_data,
                                   ygfx::spShader shader = nullptr, QOpenGLTexture * texture = nullptr);
        //explicit YMathGfxPlotSet2D(std::vector<double> * org_x_data, std::vector<double> * org_y_data);

        void appendData(std::vector<double> const & xData, std::vector<double> const & yData);
        void appendData(double x, double y);

        void setDataX(size_t index, double value);
        void setDataY(size_t index, double value);
        void setData(size_t index, double x, double y);
        void setDataA(size_t index, float value);

        gfxVector<GLfloat> const * getGFXXData()  const { return gfx_x_values; }
        gfxVector<GLfloat> const * getGFXYData()  const { return gfx_y_values; }
        std::vector<double> const * getXData()  const { return org_x_data; }
        std::vector<double> const * getYData()  const { return org_y_data; }
        double getDataX(size_t index)           const { return (org_x_data->size() > index) ? org_x_data->at(index) : 0; }
        double getDataY(size_t index)           const { return (org_y_data->size() > index) ? org_y_data->at(index) : 0; }



        void clearData();
        void removeLastData(size_t size);
    };
}
#endif // GFX_QT_MATHPLOT_SET_H
