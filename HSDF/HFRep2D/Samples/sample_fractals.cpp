#include "fractals2D.h"
#include "render2D.h"
#include "hfrep2D.h"
#include <complex>

int main(int argc, char *argv[])
{
    int resX = 512, resY = 512;
    std::vector<float> field0, field1;

    std::complex<float> c(0.285, 0.0);
    hfrep2D::fractals fractal(resX, resY);
    field0 = fractal.julia(c, 17, 2, 1);
    field1 = fractal.mandelbrot(21);

    hfrep2D::HFRepObj2D hfrep(512, 512);
    std::vector<float> hfrep0 = hfrep.calculateHFRep2D(&field0, nullptr, HYPERBOLIC_SIGMOID, 0.00001);
    std::vector<float> ddt0 = hfrep.getDDT();

    std::vector<float> hfrep1 = hfrep.calculateHFRep2D(&field1, nullptr, HYPERBOLIC_SIGMOID, 0.00001);
    std::vector<float> ddt1 = hfrep.getDDT();
    std::vector<float> sddt1 = hfrep.getSignedDDT();

    hfrep2D::render2D render;
    sf::Image img1 = render.drawIsolines(&field0, 512, 512, 0.0005, "julia_frep");
    sf::Image img2 = render.drawIsolines(&ddt0, 512, 512, 0.025, "julia_ddt");
    sf::Image img3 = render.drawIsolines(&hfrep0, 512, 512, 0.025, "julia_hfrep");
    sf::Image img4 = render.drawIsolines(&field1, 512, 512, 0.0005, "mondelbrot_frep");
    sf::Image img5 = render.drawIsolines(&ddt1, 512, 512, 0.025, "mondelbrot_ddt");
    sf::Image img5_ = render.drawIsolines(&sddt1, 512, 512, 0.025, "mondelbrot_sddt");
    sf::Image img6 = render.drawIsolines(&hfrep1, 512, 512, 0.025, "mondelbrot_hfrep");

    render.displayImage(img1);
    render.displayImage(img2);
    render.displayImage(img3);
    render.displayImage(img4);
    render.displayImage(img5);
    render.displayImage(img6);

    return 0;
}
