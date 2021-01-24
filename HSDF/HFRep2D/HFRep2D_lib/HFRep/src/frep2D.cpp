#include "frep2D.h"
#include <iostream>
#include <cmath>

namespace hfrep2D {

FRepObj2D::FRepObj2D(int resX, int resY, float scaleF)
{
    resolutionX = resX;
    resolutionY = resY;
    scale       = scaleF;
}

float FRepObj2D::triangle(glm::vec2 pos, glm::vec2 cent, float a, float b, float c)
{
    glm::vec2 uv = convertToUV(pos);
    glm::vec2 c0 = convertToUV(cent);
    a = convertToUV(scaleToNewRange(a)); // right side
    b = convertToUV(scaleToNewRange(b)); // left side
    c = convertToUV(scaleToNewRange(c)); // the base of the triangle

    float shX = scale*(uv.x - c0.x);
    float shY = scale*(uv.y - c0.y);

    float lb1 = shY + a - ( shX + b);
    float rb1 = shY + a - (-shX + b);
    return intersect_function(intersect_function(lb1, rb1), -shY+c);
}

float FRepObj2D::triangle2(glm::vec2 pos, glm::vec2 cent, const float a, const float b)
{
    glm::vec2 uv = convertToUV(pos);
    glm::vec2 c0 = convertToUV(cent);
    glm::vec2 sides = convertToUV(glm::vec2(scaleToNewRange(a), scaleToNewRange(b)));

    float rec = rectangle(pos, cent, a, b);

    float shX = scale*(uv.x - c0.x);
    float shY = scale*(uv.y - c0.y);
    float line = shY - (sides.y/sides.x)*shX;

    return intersect_function(rec, line);
}

float FRepObj2D::circle(glm::vec2 pos, glm::vec2 cent, float R)
{
    glm::vec2 uv = convertToUV(pos);
    glm::vec2 c0 = convertToUV(cent);
    R = convertToUV(scaleToNewRange(R));

    float shX = scale*(uv.x - c0.x);
    float shY = scale*(uv.y - c0.y);
    return R*R - shX*shX - shY*shY;
}

float FRepObj2D::blobby2D(glm::vec2 pos, glm::vec2 cent, float R)
{
    glm::vec2 uv = convertToUV(pos);
    glm::vec2 c0 = convertToUV(cent);
    R = convertToUV(scaleToNewRange(R));

    float shX = scale*(uv.x - c0.x);
    float shY = scale*(uv.y - c0.y);
    return R*R/(shX*shX + shY*shY);
}

float FRepObj2D::ellipticCylZ2D(glm::vec2 pos, glm::vec2 cent, float R, float a, float b)
{
    glm::vec2 uv = convertToUV(pos);
    glm::vec2 c0 = convertToUV(cent);
    R = convertToUV(scaleToNewRange(R));

    float shX = scale*(uv.x - c0.x);
    float shY = scale*(uv.y - c0.y);
    return R - std::pow(shX, 2.0f)/(a*a) - std::pow(shY, 2.0f)/(b*b);
}

float FRepObj2D::ellipsoid2D(glm::vec2 pos, glm::vec2 cent, float R, float a, float b)
{
    glm::vec2 uv = convertToUV(pos);
    glm::vec2 c0 = convertToUV(cent);
    R = convertToUV(scaleToNewRange(R));

    float shX = scale*(uv.x - c0.x);
    float shY = scale*(uv.y - c0.y);
    return R - std::pow(shX/a, 2.0f) - std::pow(shY/b, 2.0f);
}

float FRepObj2D::torusY2D(glm::vec2 pos, glm::vec2 cent, float R, float rev)
{
    glm::vec2 uv = convertToUV(pos);
    glm::vec2 c0 = convertToUV(cent);
    R = convertToUV(scaleToNewRange(R));

    float shX = scale*(uv.x - c0.x);
    float shY = scale*(uv.y - c0.y);
    return R*R - std::pow(shY, 2.0f) - std::pow(shY, 2.0f) - rev*rev +
           2.0f*rev*std::sqrt(std::pow(shX, 2.0f) + std::pow(shY, 2.0f));
}

float FRepObj2D::torusZ2D(glm::vec2 pos, glm::vec2 cent, float R, float rev)
{
    glm::vec2 uv = convertToUV(pos);
    glm::vec2 c0 = convertToUV(cent);
    R = convertToUV(scaleToNewRange(R));

    float shX = scale*(uv.x - c0.x);
    float shY = scale*(uv.y - c0.y);

    return R*R - std::pow(shX, 2.0f) - std::pow(shY, 2.0f) - rev*rev +
           2.0f*rev*std::sqrt(std::pow(shX, 2.0f) + std::pow(shY, 2.0f));
}

float FRepObj2D::rectangle(glm::vec2 pos, glm::vec2 cent, float w, float h)
{
    glm::vec2 uv  = convertToUV(pos);
    glm::vec2 c0  = convertToUV(cent);
    glm::vec2 rec = convertToUV(glm::vec2(scaleToNewRange(w), scaleToNewRange(h)));

    float shX = scale*(uv.x - c0.x);
    float shY = scale*(uv.y - c0.y);
    return intersect_function(rec.x - std::abs(shX), rec.y - std::abs(shY));
}

float FRepObj2D::heart2D(glm::vec2 pos, glm::vec2 cent)
{
    glm::vec2 uv = convertToUV(pos);
    glm::vec2 c0 = convertToUV(cent);
    float shX = scale*(uv.x - c0.x);
    float shY = scale*(uv.y - c0.y);
    return - std::pow(shX*shX + shY*shY - 1.0f, 3.0f) + shX*shX*std::pow(shY, 3.0f);
}

float FRepObj2D::decocube2D(glm::vec2 pos, glm::vec2 cent)
{
    glm::vec2 uv = convertToUV(pos);
    glm::vec2 c0 = convertToUV(cent);
    float shX = scale*(uv.x - c0.x);
    float shY = scale*(uv.y - c0.y);
    return -(std::pow(shX*shX + shY*shY - 2.0f, 2.0f) + 1.0f) *
            (std::pow(shY*shY - 2.0f, 2.0f) + std::pow(shX*shX-1.0f, 2.0f)) *
            (std::pow(shX*shX - 2.0f, 2.0f) + std::pow(shY*shY-1.0f, 2.0f)) + 50.0f/resolutionX;
}

float FRepObj2D::suriken(glm::vec2 pos, glm::vec2 cent)
{
    glm::vec2 uv = convertToUV(pos);
    glm::vec2 c0 = convertToUV(cent);
    float shX = scale*(uv.x - c0.x);
    float shY = scale*(uv.y - c0.y);

    float lb1 = shY + scaleToNewRange(358.4)/resolutionY - ( shX*2.0f + scaleToNewRange(153.6)/resolutionX);
    float rb1 = shY + scaleToNewRange(358.4)/resolutionY - (-shX*2.0f + scaleToNewRange(153.6)/resolutionX);
    float lb2 = shY - scaleToNewRange(51.2)/resolutionY  - ( shX*2.0f + scaleToNewRange(153.6)/resolutionY);
    float rb2 = shY - scaleToNewRange(51.2)/resolutionY  - (-shX*2.0f + scaleToNewRange(153.6)/resolutionY);
    float lb3 = shX + scaleToNewRange(358.4)/resolutionX - ( shY*2.0f + scaleToNewRange(153.6)/resolutionY);
    float rb3 = shX + scaleToNewRange(358.4)/resolutionX - (-shY*2.0f + scaleToNewRange(153.6)/resolutionY);
    float lb4 = shX - scaleToNewRange(51.2)/resolutionX  - ( shY*2.0f + scaleToNewRange(153.6)/resolutionY);
    float rb4 = shX - scaleToNewRange(51.2)/resolutionX  - (-shY*2.0f + scaleToNewRange(153.6)/resolutionY);

    float trian1 = intersect_function(intersect_function( lb1,  rb1), -shY+scaleToNewRange(102.4)/resolutionY);
    float trian2 = intersect_function(intersect_function(-lb2, -rb2),  shY+scaleToNewRange(102.4)/resolutionY);
    float trian3 = intersect_function(intersect_function( lb3,  rb3), -shX+scaleToNewRange(102.4)/resolutionX);
    float trian4 = intersect_function(intersect_function(-lb4, -rb4),  shX+scaleToNewRange(102.4)/resolutionX);

    return union_function(union_function(union_function(trian1, trian2), trian3), trian4);
}

float FRepObj2D::elf(glm::vec2 pos, glm::vec2 center)
{
    glm::vec2 uv = convertToUV(pos);

    //head
    glm::vec2 headC = glm::vec2(center.x, center.y - scaleToNewRange(70)/scale);
    float head      = ellipsoid2D(pos, headC, 1.0f, 6.0f, 5.5f);

    //nose
    glm::vec2 noseC = glm::vec2(center.x, center.y - scaleToNewRange(40)/scale);
    float nose = ellipsoid2D(pos, noseC, 1.0f, 0.5f, 0.5f);

    //mouth
    glm::vec2 mouthC = glm::vec2(center.x, center.y - scaleToNewRange(55)/scale);
    float mouth1   = ellipsoid2D(pos, mouthC, 1.0f, 3.0f, 2.3f);
    float mouth2   = -(center.y - scaleToNewRange(20)/scale)/resolutionY + uv.y;
    float mouth    = intersect_function(mouth1, mouth2);

    //eyes
    glm::vec2 eye1C = glm::vec2(center.x, center.y);
    float eye1    = ellipsoid2D(pos, eye1C, 1.0f, 6.0f, 5.5f);

    glm::vec2 eye2C = glm::vec2(center.x - scaleToNewRange(40)/scale, center.y - scaleToNewRange(80)/scale);
    float eye2    = ellipticCylZ2D(pos, eye2C, 1.0f, 1.5f, 1.4f);
    float eye3    = intersect_function(eye1, eye2);

    glm::vec2 eye4C = glm::vec2(center.x + scaleToNewRange(40)/scale, center.y - scaleToNewRange(80)/scale);
    float eye4    = ellipticCylZ2D(pos, eye4C, 1.0f, 1.5f, 1.4f);
    float eye5    = intersect_function(eye1, eye4);

    glm::vec2 eye6C = glm::vec2(center.x - scaleToNewRange(30)/scale, center.y - scaleToNewRange(70)/scale);
    float eye6    = ellipsoid2D(pos, eye6C, 1.0f, 0.3f, 0.3f);
    float eye7    = subtract_function(eye3, eye6);

    glm::vec2 eye8C = glm::vec2(center.x + scaleToNewRange(30)/scale, center.y - scaleToNewRange(70)/scale);
    float eye8    = ellipsoid2D(pos, eye8C, 1.0f, 0.3f, 0.3f);
    float eye9    = subtract_function(eye5, eye8);

    //head final
    float fin_head0 = union_function(eye9, eye7);
    float fin_head2 = subtract_function(head, mouth);
    float fin_head3 = subtract_function(fin_head2, nose);
    float fin_head  = subtract_function(fin_head3, fin_head0);

    //body
    glm::vec2 bodyC = glm::vec2(center.x, center.y + scaleToNewRange(25)/scale);
    float body1 = ellipsoid2D(pos, bodyC, 1.0f, 4.5f, 6.0f);
    float body2 = intersect_function(intersect_function(body1, (center.y + scaleToNewRange(30)/scale)/resolutionY+uv.y),
                                                              -(center.y + scaleToNewRange(40)/scale)/resolutionY+uv.y);

    //neck
    glm::vec2 neckC = glm::vec2(center.x, center.y + scaleToNewRange(32)/scale);
    float neck = ellipsoid2D(pos, neckC, 1.0f, 2.5f, 0.8f);

    //bell
    glm::vec2 bellC = glm::vec2(center.x, center.y + scaleToNewRange(90)/scale);
    float bell  = circle(pos, bellC, 12.0f);
    float body3 = union_function(body2, neck);

    //belly
    glm::vec2 bellyC1 = glm::vec2(center.x, center.y + scaleToNewRange(80)/scale);
    float belly1 = ellipsoid2D(pos, bellyC1, 1.0f, 4.5, 6.0);

    glm::vec2 bellyC2 = glm::vec2(center.x, center.y + scaleToNewRange(135)/scale);
    float belly2 = ellipticCylZ2D(pos, bellyC2, 1.0f, 0.8f, 0.8f);
    float belly3 = intersect_function(belly1, belly2);

    glm::vec2 bellyC4 = glm::vec2(center.x, center.y + scaleToNewRange(50)/scale);
    float belly4 = ellipticCylZ2D(pos, bellyC4, 1.0f, 2.0f, 1.6f);

    glm::vec2 bellyC5 = glm::vec2(center.x, center.y + scaleToNewRange(115)/scale);
    float belly5 = ellipsoid2D(pos, bellyC5, 1.0f, 1.1f, 1.1f);
    float belly6 = -(center.y + scaleToNewRange(20)/scale)/resolutionY + uv.y;

    float fin_body0 = intersect_function(belly4, belly6);
    float fin_body1 = union_function(fin_body0, belly5);
    float fin_body2 = subtract_function(body3, fin_body1);
    float fin_body3 = union_function(fin_body2, belly3);
    float fin_body  = union_function(fin_body3, bell);

    //legs and feet
    glm::vec2 l_legC = glm::vec2(center.x + scaleToNewRange(70)/scale, center.y + scaleToNewRange(160)/scale);
    float l_leg    = ellipsoid2D(pos, l_legC, 1.0f, 3.0f, 1.5f);
    glm::vec2 r_legC = glm::vec2(center.x - scaleToNewRange(70)/scale, center.y + scaleToNewRange(160)/scale);
    float r_leg    = ellipsoid2D(pos, r_legC, 1.0f, 3.0, 1.5);

    glm::vec2 l_footC = glm::vec2(center.x - scaleToNewRange(40)/scale, center.y + scaleToNewRange(140)/scale);
    float l_foot    = ellipsoid2D(pos, l_footC, 1.0f, 1.0, 1.0f);
    glm::vec2 r_footC = glm::vec2(center.x + scaleToNewRange(40)/scale, center.y + scaleToNewRange(140)/scale);
    float r_foot    = ellipsoid2D(pos, r_footC, 1.0f, 1.0f, 1.0f);

    float legs0 = union_function(l_leg, l_foot);
    float legs1 = union_function(r_leg, r_foot);
    float legs  = union_function(legs0, legs1);

    //right mustache
    float rh = scaleToNewRange(0.15f);
    glm::vec2 r_mustC1 = glm::vec2(center.x - scaleToNewRange(100)/scale, center.y + scaleToNewRange(30)/scale);
    float r_must1      = torusZ2D(pos, r_mustC1, 15.0f, rh);
    glm::vec2 l_mustC1 = glm::vec2(center.x + scaleToNewRange(100)/scale, center.y + scaleToNewRange(30)/scale);
    float l_must1      = torusZ2D(pos, l_mustC1, 15.0f, rh);

    float must11 = intersect_function(uv.y, (center.y + scaleToNewRange(5)/scale)/resolutionY - uv.y);
    float must22 = union_function(l_must1, r_must1);
    float mustache  = intersect_function(intersect_function(must22, must11), uv.y);

    //hands
    glm::vec2 r_handC = glm::vec2(center.x + scaleToNewRange(150)/scale, center.y + scaleToNewRange(70)/scale);
    float r_hand = circle(pos, r_handC, 30.0f);
    glm::vec2 l_handC = glm::vec2(center.x - scaleToNewRange(150)/scale, center.y + scaleToNewRange(70)/scale);
    float l_hand  = circle(pos, l_handC, 30.0f);

    float ude2   = uv.y + (center.y + scaleToNewRange(60)/scale)/resolutionY;
    float ude3   = (center.y + scaleToNewRange(60)/scale)/resolutionY - uv.y;
    glm::vec2 udeC = glm::vec2(center.x, center.y + scaleToNewRange(180)/scale);
    float ude1   = torusZ2D(pos, udeC, 30.0f, 0.3f);
    float te1    = intersect_function(intersect_function(ude1, ude2), ude3);
    float te2    = union_function(union_function(te1, r_hand), l_hand);

    //final model
    float elf0 = union_function(fin_head, fin_body);
    float elf1 = union_function(elf0, legs);
    float elf2 = union_function(elf1, te2);
    float elf  = union_function(elf2, mustache);

    return elf;
}

float FRepObj2D::bat(glm::vec2 pos, glm::vec2 center)
{
    //glm::vec2 center = glm::vec2(resolutionX/2.0f, resolutionY/2.0f);

    float rec0 = rectangle(pos, center, 50, 30); //main body
    float rec1 = rectangle(pos, glm::vec2(center.x + scaleToNewRange(80)/scale,
                                          center.y - scaleToNewRange(5.5)/scale), 40, 36);
    float rec2 = rectangle(pos, glm::vec2(center.x - scaleToNewRange(80)/scale,
                                          center.y - scaleToNewRange(5.5)/scale), 40, 36);
    float rec3 = rectangle(pos, glm::vec2(center.x + scaleToNewRange(85)/scale,
                                          center.y - scaleToNewRange(45)/scale), 35, 9); //right part of the wing
    float rec4 = rectangle(pos, glm::vec2(center.x - scaleToNewRange(85)/scale,
                                          center.y - scaleToNewRange(45)/scale), 35, 9); //left part of the wing
    float rec5 = rectangle(pos, glm::vec2(center.x, center.y - scaleToNewRange(35)/scale), 12, 6);  //head

    float tri0 = triangle2(getRotatedCoords(pos, 45), getRotatedCoords(glm::vec2(center.x, center.y + scaleToNewRange(20)/scale), 45),
                                                                              40, 40); //bat tale, bottom triangle
    float tri1 = triangle2(pos, glm::vec2(center.x - scaleToNewRange(34)/scale, center.y - scaleToNewRange(33)/scale),
                                                                                7, 7);                                              //supportive triangle (bot) for left wing, internal
    float tri2 = triangle2(getRotatedCoords(pos, 90), getRotatedCoords(glm::vec2(center.x + scaleToNewRange(34)/scale, center.y - scaleToNewRange(34)/scale), 90),
                                                    7, 7);   //supportive triangle (bot) for right wing, internal
    float tri3 = triangle2(pos, glm::vec2(center.x - scaleToNewRange(44)/scale, center.y - scaleToNewRange(45)/scale),
                                                    7, 9);   //supportive triangle (up) for left wing, internal
    float tri4 = triangle2(getRotatedCoords(pos, 90), getRotatedCoords(glm::vec2(center.x + scaleToNewRange(44)/scale, center.y - scaleToNewRange(45)/scale), 90),
                                                    9, 7);   //supportive triangle (up) for right wing, internal
    float tri5 = triangle2(pos, glm::vec2(center.x + scaleToNewRange(8)/scale, center.y - scaleToNewRange(50)/scale),
                                                    4, 10);  //right ear
    float tri6 = triangle2(getRotatedCoords(pos, 90), getRotatedCoords(glm::vec2(center.x - scaleToNewRange(8)/scale, center.y - scaleToNewRange(50)/scale), 90),
                                                   10, 4);  //left ear
    float tri7 = triangle2(getRotatedCoords(pos, 180), getRotatedCoords(glm::vec2(center.x - scaleToNewRange(152)/scale, center.y - scaleToNewRange(19.5)/scale), 180),
                                                  41, 35); //left wing
    float tri8 = triangle2(getRotatedCoords(pos, 270), getRotatedCoords(glm::vec2(center.x + scaleToNewRange(150)/scale, center.x - scaleToNewRange(19.5)/scale), 270),
                                                  35, 41); //right wing

    float union1  = union_function(rec0, rec1);
    float union2  = union_function(rec2, union1);
    float union3  = union_function(rec3, union2);
    float union4  = union_function(rec4, union3);
    float union5  = union_function(rec5, union4);
    float union6  = union_function(tri0, union5);
    float union7  = union_function(tri1, union6);
    float union8  = union_function(tri2, union7);
    float union9  = union_function(tri3, union8);
    float union10 = union_function(tri4, union9);
    float union11 = union_function(tri5, union10);
    float union12 = union_function(tri6, union11);
    float union13 = union_function(tri7, union12);
    float bat     = union_function(tri8, union13);

    return bat;
}

float FRepObj2D::trebleClef(glm::vec2 pos, glm::vec2 center)
{
    //glm::vec2 center = glm::vec2(resolutionX/2.0f, resolutionY/2.0f);

    //constructing bottom part
    float rec_base = rectangle(pos, glm::vec2(center.x, center.y - scaleToNewRange(20)), scaleToNewRange(15), 120);
    float ark1 = circle(pos, glm::vec2(center.x - scaleToNewRange(20)/scale, center.y + scaleToNewRange(95)/scale), 35);
    float ark2 = circle(pos, glm::vec2(center.x - scaleToNewRange(50)/scale, center.y + scaleToNewRange(75)/scale), 40);
    float ark3 = circle(pos, glm::vec2(center.x - scaleToNewRange(30)/scale, center.y + scaleToNewRange(95)/scale), 35);
    float ark4 = circle(pos, glm::vec2(center.x - scaleToNewRange(20)/scale, center.y + scaleToNewRange(70)/scale), 35);
    float circ_bot = circle(pos, glm::vec2(center.x - scaleToNewRange(40)/scale, center.y + scaleToNewRange(85)/scale), 20);

    float arkB1 = subtract_function(ark1, ark2);
    float arkB2 = subtract_function(ark3, ark4);

    float union1  = union_function(arkB1, rec_base);
    float union2  = union_function(arkB2, union1);
    float fin_bot = union_function(circ_bot, union2);

    //constructing middle part
    float ark5 = circle(pos, glm::vec2(center.x + scaleToNewRange(12)/scale, center.y - scaleToNewRange(10)/scale), 60);
    float ark6 = circle(pos, glm::vec2(center.x - scaleToNewRange(5)/scale, center.y - scaleToNewRange(5)/scale), 49);
    float ark7 = circle(pos, glm::vec2(center.x, center.y - scaleToNewRange(28)/scale), 80);
    float ark8 = circle(pos, glm::vec2(center.x + scaleToNewRange(15)/scale, center.y - scaleToNewRange(28)/scale), 70);

    float arkM1 = subtract_function(ark5, ark6);
    float arkM2 = subtract_function(ark7, ark8);

    float union3     = union_function(arkM1, fin_bot);
    float fin_middle = union_function(arkM2, union3);

    //constructing upper part
    float ark9  = circle(pos, glm::vec2(center.x, center.y - scaleToNewRange(155)/scale), 57);
    float ark10 = circle(pos, glm::vec2(center.x - scaleToNewRange(30)/scale, center.y - scaleToNewRange(169)/scale), 65);
    float ark11 = circle(pos, glm::vec2(center.x + scaleToNewRange(44)/scale, center.y - scaleToNewRange(150)/scale), 60);
    float ark12 = circle(pos, glm::vec2(center.x + scaleToNewRange(87)/scale, center.y - scaleToNewRange(160)/scale), 75);
    float rec_helper1 = rectangle(pos, glm::vec2(center.x, center.y - scaleToNewRange(85)/scale), scaleToNewRange(100), scaleToNewRange(48));
    float circ_top    = circle(pos, glm::vec2(center.x + scaleToNewRange(27)/scale, center.y - scaleToNewRange(201)/scale), 5);

    float arkU1  = subtract_function(ark9, ark10);
    float arkU2  = subtract_function(ark11, ark12);
    float arkU2f = subtract_function(arkU2, rec_helper1);

    float union4 = union_function(arkU1, fin_middle);
    float union5 = union_function(circ_top, union4);
    float trebleclef = union_function(arkU2f, union5);

    return trebleclef;
}

float FRepObj2D::bounded_blending(float f1, float f2, float a0, float a1, float a2, float a3,
                                  float time, float alpha, float m)
{
      f1 = intersect_function(f1, -time, alpha, m);
      f2 = intersect_function(f2, time - 1.0f, alpha, m);
      float f3 = intersect_function(time + 10.0f, 10.0f - time, alpha, m);

      float r1 = (f1/a1)*(f1/a1) + (f2/a2)*(f2/a2);
      float r2 = 0.0f;

      if(f3 > 0.0f)
        r2 = (f3/a3) * (f3/a3);

      float rr = 0.0f;
      if(r1 > 0.0f)
        rr = r1 / (r1 + r2);

      float d = 0.0f;
      if(rr < 1.0f)
        d = a0 * (1.0f - rr)*(1.0f - rr)*(1.0f - rr) / (1.0f + rr);

      return union_function(f1, f2, alpha, m) + d;
}

float FRepObj2D::blending(float f1, float f2, float a1, float a2)
{

}

float FRepObj2D::blending(float f1, float f2, float a0, float a1, float a2, float a3, float time)
{

}



float FRepObj2D::union_function_R0(float f1, float f2, float n)
{
    float result = 0.0f;

    if(f1 > 0.0f && f2 > 0.0f)
       result = std::pow(std::pow(f1, n) + std::pow(f2, n), 1.0f/n);
    else if(f1 <= 0.0f && f2 >= 0.0f)
       result = f2;
    else if(f1 >= 0.0f && f2 <= 0.0f)
       result = f1;
    else if(f1 < 0.0f && f2 < 0.0f)
       result = std::pow(-1.0f, n+1.0f) * f1*f2 * std::pow(std::pow(f1, n) + std::pow(f2, n), -1.0f/n);

    return result;
}

glm::vec2 FRepObj2D::findZeroLevelSetInterval(std::vector<float> field, int numElemToAverage)
{
    std::vector<float> posVals, negVals;

    for(size_t i = 0; i < field.size(); i++)
    {
        if(field[i] >= 0)
            posVals.push_back(field[i]);
    }

    std::vector<float> minPosV;
    for(int j = 0; j < numElemToAverage; j++)
    {
        std::vector<float>::iterator minPos = std::min_element(posVals.begin(), posVals.end());
        minPosV.push_back(*minPos);
        posVals.erase(std::remove(posVals.begin(), posVals.end(), *minPos), posVals.end());
    }

    float minPos, minPsum = 0;
    for(size_t i = 0; i < minPosV.size(); i++)
        minPsum += minPosV[i];

    minPos = minPsum / static_cast<float>(minPosV.size());

    glm::vec2 result = glm::vec2(0.0f, minPos);
    return result;
}

std::vector<float> FRepObj2D::scaleFunction(const std::vector<float> field, const float factor)
{
    std::vector<float> scaledField;
    std::transform(field.begin(), field.end(), std::back_inserter(scaledField), std::bind(std::multiplies<float>(), factor, std::placeholders::_1));
    return scaledField;
}

float FRepObj2D::intersect_function(float f1, float f2, float alpha, float m)
{ return (1.0f/(1.0f+alpha))*(f1 + f2 - std::sqrt(f1 * f1 + f2 * f2 - 2*alpha*f1*f2)*std::pow(f1*f1+f2*f2, m/2.0f)); }

float FRepObj2D::union_function(float f1, float f2, float alpha, float m)
{ return (1.0f/(1.0f+alpha))*(f1 + f2 + std::sqrt(f1 * f1 + f2 * f2 - 2*alpha*f1*f2)*std::pow(f1*f1+f2*f2, m/2.0f)); }

float FRepObj2D::subtract_function(float f1, float f2, float alpha, float m)
{ return intersect_function(f1, -f2, alpha, m); }

std::vector<float> FRepObj2D::getFRep2D(std::function<float(glm::vec2)> fun)
{
    frep.clear();
    for(int y = 0; y < resolutionY; y++)
        for(int x = 0; x < resolutionX; x++)
            frep.push_back(fun(glm::vec2(x, y)));

    return frep;
}

std::vector<float> FRepObj2D::getFRep2D(glm::vec2 cent, std::function<float(glm::vec2, glm::vec2)> fun)
{
    frep.clear();
    for(int y = 0; y < resolutionY; y++)
        for(int x = 0; x < resolutionX; x++)
            frep.push_back(fun(glm::vec2(x, y), cent));

    return frep;
}

std::vector<float> FRepObj2D::getFRep2D(glm::vec2 cent, float R,
                                        std::function<float(glm::vec2, glm::vec2, float)> fun)
{
    frep.clear();
    for(int y = 0; y < resolutionY; y++)
        for(int x = 0; x < resolutionX; x++)
            frep.push_back(fun(glm::vec2(x, y), cent, R));

    return frep;
}

std::vector<float> FRepObj2D::getFRep2D(glm::vec2 cent, float p1, float p2,
                                        std::function<float(glm::vec2, glm::vec2, float, float)> fun)
{
    frep.clear();
    for(int y = 0; y < resolutionY; y++)
        for(int x = 0; x < resolutionX; x++)
            frep.push_back(fun(glm::vec2(x, y), cent, p1, p2));

    return frep;
}

std::vector<float> FRepObj2D::getFRep2D(glm::vec2 cent, float p1, float p2, float p3,
                                        std::function<float(glm::vec2, glm::vec2, float, float, float)> fun)
{
    frep.clear();
    for(int y = 0; y < resolutionY; y++)
        for(int x = 0; x < resolutionX; x++)
            frep.push_back(fun(glm::vec2(x, y), cent, p1, p2, p3));

    return frep;
}

std::vector<float> FRepObj2D::getFRep2D(std::vector<float> f1, std::vector<float> f2, float alpha, float m,
                                         std::function<float (float, float, float, float)> fun)
{
    frep.clear();
    for(int i = 0; i < resolutionX*resolutionY; i++)
        frep.push_back(fun(f1[i], f2[i], alpha, m));

    return frep;
}

std::vector<float> FRepObj2D::getFRep2D(std::vector<float> f1, std::vector<float> f2, float n, std::function<float (float, float, float)> fun)
{
    frep.clear();
    for(int i = 0; i < resolutionX*resolutionY; i++)
        frep.push_back(fun(f1[i], f2[i], n));

    return frep;
}

glm::vec2 FRepObj2D::getRotatedCoords(glm::vec2 inCoords, const float angle)
{
    float angle0 = static_cast<float>((angle/180.0f) * M_PI);

    float rotX = inCoords.x*cosf(angle0) - inCoords.y*sinf(angle0);
    float rotY = inCoords.x*sinf(angle0) + inCoords.y*cosf(angle0);
    return glm::vec2(rotX, rotY);
}

std::vector<float> FRepObj2D::getRotatedFrep2D(glm::vec2 cent, float w, float h,
                                                float angle, std::function<float (glm::vec2, glm::vec2, float, float)> fun)
{
    frep.clear();
    glm::vec2 c0 = getRotatedCoords(cent, angle);

    for(int y = 0; y < resolutionY; y++)
        for(int x = 0; x < resolutionX; x++)
            frep.push_back(fun(getRotatedCoords(glm::vec2(x,y), angle), c0, w, h));

    return frep;
}

std::vector<float> FRepObj2D::getRotatedFrep2D(glm::vec2 cent, float a, float b, float c, float angle,
                                                std::function<float (glm::vec2, glm::vec2, float, float, float, float)> fun)
{
    frep.clear();
    glm::vec2 c0 = getRotatedCoords(cent, angle);

    for(int y = 0; y < resolutionY; y++)
        for(int x = 0; x < resolutionX; x++)
            frep.push_back(fun(getRotatedCoords(glm::vec2(x, y), angle), c0, a, b, c, angle));

    return frep;
}

} //namespace frep
