#include "frep2D.h"
#include "hfrep2D.h"
#include "render2D.h"
#include "timer.hpp"

#include <vector>
using namespace std::placeholders;

int main(int argc, char *argv[])
{
    prof::timer Time;
    Time.Start();

    int resX = 512, resY = 512;
    hfrep2D::FRepObj2D frep(resX, resY, 1.0);

    auto rectangle = std::bind(&hfrep2D::FRepObj2D::rectangle, frep, _1, _2, _3, _4);
    auto circle    = std::bind(&hfrep2D::FRepObj2D::circle, frep, _1, _2, _3);
    auto union_f   = std::bind(&hfrep2D::FRepObj2D::union_function, frep, _1, _2, _3, _4);
    auto intersect_f = std::bind(&hfrep2D::FRepObj2D::intersect_function, frep, _1, _2, _3, _4);
    auto subtract_f  = std::bind(&hfrep2D::FRepObj2D::subtract_function, frep, _1, _2, _3, _4);

    //head_base
    std::vector<float> head0 = frep.getFRep2D(glm::vec2(250, 110), 50, 50, rectangle);
    std::vector<float> head1 = frep.getFRep2D(glm::vec2(200, 110), 50, circle);
    std::vector<float> head2 = frep.getFRep2D(glm::vec2(300, 110), 50, circle);

    std::vector<float> head3 = frep.getFRep2D(head0, head1, 0, 0, union_f);
    std::vector<float> head4 = frep.getFRep2D(head3, head2, 0, 0, union_f);

    //head eyes
    std::vector<float> eye1_l = frep.getFRep2D(glm::vec2(205, 110), 30, circle);
    std::vector<float> eye2_r = frep.getFRep2D(glm::vec2(295, 110), 30, circle);
    std::vector<float> eye3_l = frep.getFRep2D(glm::vec2(205, 110), 20, circle);
    std::vector<float> eye4_r = frep.getFRep2D(glm::vec2(295, 110), 20, circle);
    std::vector<float> eye5_l = frep.getFRep2D(glm::vec2(198, 99), 9, circle);
    std::vector<float> eye6_r = frep.getFRep2D(glm::vec2(302, 99), 9, circle);

    std::vector<float> head_eye1 = frep.getFRep2D(head4, eye1_l, 0, 0, subtract_f);
    std::vector<float> head_eye2 = frep.getFRep2D(head_eye1, eye2_r, 0, 0, subtract_f);
    std::vector<float> head_eye3 = frep.getFRep2D(head_eye2, eye3_l, 0, 0, union_f);
    std::vector<float> head_eye4 = frep.getFRep2D(head_eye3, eye4_r, 0, 0, union_f);
    std::vector<float> head_eye5 = frep.getFRep2D(head_eye4, eye5_l, 0, 0, subtract_f);
    std::vector<float> head = frep.getFRep2D(head_eye5, eye6_r, 0, 0, subtract_f);

    //neck
    std::vector<float> neck = frep.getFRep2D(glm::vec2(250, 175), 15, 20, rectangle);
    std::vector<float> head_neck = frep.getFRep2D(head, neck, 0, 0, union_f);

    //body, top
    std::vector<float> body1 = frep.getFRep2D(glm::vec2(250, 250), 110, 60, rectangle);
    std::vector<float> body2_l = frep.getFRep2D(glm::vec2(151, 203), 20, circle);
    std::vector<float> body2_r = frep.getFRep2D(glm::vec2(347, 203), 20, circle);
    std::vector<float> body3_l = frep.getFRep2D(glm::vec2(164, 214), 25, circle);
    std::vector<float> body3_r = frep.getFRep2D(glm::vec2(336, 214), 25, circle);

    //body, bottom
    std::vector<float> body4_l = frep.getFRep2D(glm::vec2(151, 297), 20, circle);
    std::vector<float> body4_r = frep.getFRep2D(glm::vec2(347, 297), 20, circle);
    std::vector<float> body5_l = frep.getFRep2D(glm::vec2(164, 284), 25, circle);
    std::vector<float> body5_r = frep.getFRep2D(glm::vec2(336, 284), 25, circle);

    std::vector<float> body_p1 = frep.getFRep2D(head_neck, body1, 0, 0, union_f);
    std::vector<float> body_p2 = frep.getFRep2D(body_p1, body2_l, 0, 0, subtract_f);
    std::vector<float> body_p3 = frep.getFRep2D(body_p2, body2_r, 0, 0, subtract_f);
    std::vector<float> body_p4 = frep.getFRep2D(body_p3, body3_l, 0, 0, union_f);
    std::vector<float> body_p5 = frep.getFRep2D(body_p4, body3_r, 0, 0, union_f);

    std::vector<float> body_p6 = frep.getFRep2D(body_p5, body4_l, 0, 0, subtract_f);
    std::vector<float> body_p7 = frep.getFRep2D(body_p6, body4_r, 0, 0, subtract_f);
    std::vector<float> body_p8 = frep.getFRep2D(body_p7, body5_l, 0, 0, union_f);
    std::vector<float> body = frep.getFRep2D(body_p8, body5_r, 0, 0, union_f);

    //arms
    std::vector<float> shoulder_l = frep.getFRep2D(glm::vec2(140, 230), 25, circle);
    std::vector<float> shoulder_r = frep.getFRep2D(glm::vec2(358, 230), 25, circle);
    std::vector<float> arm1_l = frep.getRotatedFrep2D(glm::vec2(106, 205), 15, 40, 45, rectangle);
    std::vector<float> arm1_r = frep.getRotatedFrep2D(glm::vec2(395, 260), 15, 40, 45, rectangle);
    std::vector<float> arm2_l = frep.getFRep2D(glm::vec2(80, 180), 20, circle);
    std::vector<float> arm2_r = frep.getFRep2D(glm::vec2(420, 285), 20, circle);

    std::vector<float> armW1_l = frep.getRotatedFrep2D(glm::vec2(72, 155), 12, 20, 25, rectangle);
    std::vector<float> armW2_l = frep.getRotatedFrep2D(glm::vec2(60, 180), 12, 20, -85, rectangle);
    std::vector<float> armW3_l = frep.getRotatedFrep2D(glm::vec2(56, 132), 12, 16, 45, rectangle);
    std::vector<float> armW4_l = frep.getRotatedFrep2D(glm::vec2(34, 175), 12, 16, 55, rectangle);

    std::vector<float> armW1_r = frep.getRotatedFrep2D(glm::vec2(445, 275), 12, 16, 105, rectangle);
    std::vector<float> armW2_r = frep.getRotatedFrep2D(glm::vec2(465, 280), 12, 16, 55, rectangle);
    std::vector<float> armW3_r = frep.getRotatedFrep2D(glm::vec2(430, 304), 12, 16, 25, rectangle);
    std::vector<float> armW4_r = frep.getRotatedFrep2D(glm::vec2(442, 318), 12, 16, 55, rectangle);

    std::vector<float> body_sh1 = frep.getFRep2D(body, shoulder_l, 0, 0, union_f);
    std::vector<float> body_sh2 = frep.getFRep2D(body_sh1, shoulder_r, 0, 0, union_f);
    std::vector<float> body_arm1 = frep.getFRep2D(body_sh2, arm1_l, 0, 0, union_f);
    std::vector<float> body_arm2 = frep.getFRep2D(body_arm1, arm1_r, 0, 0, union_f);
    std::vector<float> body_arm3 = frep.getFRep2D(body_arm2, arm2_l, 0, 0, union_f);
    std::vector<float> body_arm4 = frep.getFRep2D(body_arm3, arm2_r, 0, 0, union_f);

    std::vector<float> body_arm5 = frep.getFRep2D(body_arm4, armW1_l, 0, 0, union_f);
    std::vector<float> body_arm6 = frep.getFRep2D(body_arm5, armW2_l, 0, 0, union_f);
    std::vector<float> body_arm7 = frep.getFRep2D(body_arm6, armW3_l, 0, 0, union_f);
    std::vector<float> body_arm8 = frep.getFRep2D(body_arm7, armW4_l, 0, 0, union_f);

    std::vector<float> body_arm9  = frep.getFRep2D(body_arm8, armW1_r, 0, 0, union_f);
    std::vector<float> body_arm10 = frep.getFRep2D(body_arm9, armW2_r, 0, 0, union_f);
    std::vector<float> body_arm11 = frep.getFRep2D(body_arm10, armW3_r, 0, 0, union_f);
    std::vector<float> body_with_arms = frep.getFRep2D(body_arm11, armW4_r, 0, 0, union_f);

    //legs
    std::vector<float> leg1_l = frep.getFRep2D(glm::vec2(195, 310), 25, circle);
    std::vector<float> leg1_r = frep.getFRep2D(glm::vec2(305, 310), 25, circle);
    std::vector<float> leg2_l = frep.getFRep2D(glm::vec2(195, 350), 15, 40, rectangle);
    std::vector<float> leg2_r = frep.getFRep2D(glm::vec2(305, 350), 15, 40, rectangle);
    std::vector<float> leg3_l = frep.getFRep2D(glm::vec2(195, 410), 40, circle);
    std::vector<float> leg3_r = frep.getFRep2D(glm::vec2(305, 410), 40, circle);
    std::vector<float> flat   = frep.getFRep2D(glm::vec2(250, 200), resX, 200, rectangle);

    std::vector<float> body_legs1 = frep.getFRep2D(body_with_arms, leg1_l, 0, 0, union_f);
    std::vector<float> body_legs2 = frep.getFRep2D(body_legs1, leg1_r, 0, 0, union_f);
    std::vector<float> body_legs3 = frep.getFRep2D(body_legs2, leg2_l, 0, 0, union_f);
    std::vector<float> body_legs4 = frep.getFRep2D(body_legs3, leg2_r, 0, 0, union_f);
    std::vector<float> body_legs5 = frep.getFRep2D(body_legs4, leg3_l, 0, 0, union_f);
    std::vector<float> body_legs6 = frep.getFRep2D(body_legs5, leg3_r, 0, 0, union_f);
    std::vector<float> frep_robot = frep.getFRep2D(flat, body_legs6, 0, 0, intersect_f);

    Time.End("FRep generation: ");

    Time.resetIter();
    Time.Start();
    hfrep2D::HFRepObj2D hfrep(resX, resY);
    std::vector<float> robot = hfrep.calculateHFRep2D(&frep_robot, nullptr, HYPERBOLIC_SIGMOID, 0.00001);
    Time.End("HFRep generation: ");

    std::vector<float> robot_sdf = hfrep.getSignedDDT();
    std::vector<float> robot_udf = hfrep.getDDT();

    std::vector<float> scaledF = frep.scaleFunction(frep_robot, 120);
    hfrep2D::render2D render;
    sf::Image im0 = render.drawIsolines(&scaledF, resX, resY, 0.02, "frep", 0.04);
    sf::Image im1 = render.drawIsolines(&robot, resX, resY, 0.05, "hfrep");
    sf::Image im2 = render.drawIsolines(&robot_sdf, resX, resY, 0.05, "sdf");
    sf::Image im3 = render.drawIsolines(&robot_udf, resX, resY, 0.05, "sdf");

    render.displayImage(im0);
    render.displayImage(im1);

    auto elf = std::bind(&hfrep2D::FRepObj2D::elf, frep, _1, _2);
    std::vector<float> frep_elf = frep.getFRep2D(glm::vec2(resX/2, resY/2), elf);
    std::vector<float> hfrep_elf = hfrep.calculateHFRep2D(&frep_elf, nullptr, HYPERBOLIC_SIGMOID, 0.00001);
    std::vector<float> sdf_elf = hfrep.getSignedDDT();

    std::vector<float> frep_elf1 = frep.scaleFunction(frep_elf, 500);
    render.drawIsolines(&frep_elf1, resX, resY, 0.02, "elf_frep", 0.04);
    render.drawIsolines(&sdf_elf, resX, resY, 0.05, "elf_sdf");

    return 0;
}
