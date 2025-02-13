#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include "VectorUtils.hpp"
#include "Rendering.hpp"
#include "Sphere.hpp"
#include "Ray.hpp"
#include "Camera.hpp"
#include "Material.hpp"
#include <string>
#include <fstream>
#include <exception>
#include <sstream>

using namespace Catch::Matchers;

std::vector<std::vector<std::array<float, 3>>> loadImage(std::string filename, int w = 100, int h = 100)
{
    std::vector<std::vector<std::array<float, 3>>> image;
    std::ifstream image_file;
    image_file.open(filename);
    if (image_file)
    {
        std::string line;
        int line_num = 0;

        // ignore header line
        std::getline(image_file, line);

        // get dimensions
        std::getline(image_file, line);
        std::istringstream line_stream(line);
        int width, height;
        line_stream >> width;
        line_stream >> height;

        if ((width != w) || (height != h))
        {
            throw std::runtime_error("Dimensions of the image are not as expected");
        }

        image.resize(width);
        for (auto &col : image)
        {
            col.resize(height);
        }

        // ignore  pixel limit
        std::getline(image_file, line);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                if (std::getline(image_file, line))
                {
                    std::istringstream pixel_stream(line);
                    pixel_stream >> image[x][y][0];
                    pixel_stream >> image[x][y][1];
                    pixel_stream >> image[x][y][2];
                }
                else
                {
                    throw std::runtime_error("Ran out of pixel data.");
                }
            }
        }
    }
    else
    {
        throw std::runtime_error("File " + filename + " not found.");
    }

    return image;
}

TEST_CASE("Test Camera Creation", "[Test Object Constructors]")
{
    using namespace VecUtils;
    Camera *cam;
    REQUIRE_NOTHROW(cam = new Camera(3, 3));
    delete cam;
    REQUIRE_NOTHROW(cam = new Camera(100, 100, {0, 0, -5}));
    REQUIRE(cam->position == Vec3{0, 0, -5});
    delete cam;
}

TEST_CASE("Test Ray Creation", "[Test Object Constructors]")
{
    Ray *ray;
    REQUIRE_NOTHROW(ray = new Ray({0,0,0},{1,0,0}));
    delete ray;

    Camera cam(5, 5);
    REQUIRE_NOTHROW(ray = new Ray(cam.position, {1,0,0}));
    delete ray;
}

TEST_CASE("Test Material Creation", "[Test Object Constructors]")
{
    Material *mat;
    REQUIRE_NOTHROW(mat = new Material());
    delete mat;
    REQUIRE_NOTHROW(mat = new Material(255, 255, 0));
    delete mat;
}

TEST_CASE("Test Sphere Creation", "[Test Object Constructors]")
{
    Sphere *s;
    REQUIRE_NOTHROW(s = new Sphere(5, {0, 0, 0}));
    delete s;

    Material mat(255, 0, 0);
    REQUIRE_NOTHROW(s = new Sphere(5, {1, 1, 10}, mat));
    delete s;
}

TEST_CASE("Test IntersectionData Creation", "[Test Object Constructors]")
{
    IntersectionData *id;
    REQUIRE_NOTHROW(id = new IntersectionData);
}

TEST_CASE("Test Small Intersection Grid", "[Test Sphere Intersection]")
{
    using namespace VecUtils;

    Sphere S(1, {0, 0, 0});
    Camera cam(3, 3);

    Ray ray(cam.position, Render::genRayDirection(1, 1, cam));
    IntersectionData id;
    REQUIRE(S.Intersect(ray, id));
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            if( (i==1) & (j==1) ) continue;
            Ray newRay(cam.position, Render::genRayDirection(i, j, cam));
            REQUIRE(!S.Intersect(newRay, id));
        }
    }
}

TEST_CASE("Test Sphere Pointer", "[Test Rendering]")
{
    using namespace VecUtils;

    Material mat(124, 17, 192);

    Sphere S(5, {0, 0, 0});
    Camera cam(3, 3);

    Ray ray(cam.position, Render::genRayDirection(1, 1, cam));
    IntersectionData id;
    REQUIRE(S.Intersect(ray, id));

    REQUIRE(id.getObject() == &S);
}

TEST_CASE("Test Rendering", "[Test Image]")
{
    using namespace VecUtils;

    Material mat(192, 42, 231);
    Sphere S(5, {0, 0, 0}, mat);
    Camera cam(3, 3);

    auto pixels = Render::genImage(cam, S);

    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            if( (i==1) & (j==1) )
            {
                REQUIRE(pixels[i][j][0] == 192);
                REQUIRE(pixels[i][j][1] == 42);
                REQUIRE(pixels[i][j][2] == 231);
            }
            else
            {
                REQUIRE(pixels[i][j][0] == 0);
                REQUIRE(pixels[i][j][1] == 0);
                REQUIRE(pixels[i][j][2] == 0);
            }
        }
    }
}

TEST_CASE("Test Straight On", "[Test Image]")
{
    Sphere sphere(5, {0,0,0}, Material(255, 255, 0));
    int w = 100; 
    int h = 100;
    Camera cam(w, h);
    auto image_data = Render::genImage(cam, sphere);
    auto straight = loadImage("data/straight.pbm", w, h);

    double diff;
    for(int px = 0; px < w; px++)
    {
        for(int py = 0; py < h; py++)
        {
            diff += image_data[px][py][0] - straight[px][py][0];
            diff += image_data[px][py][1] - straight[px][py][1];
            diff += image_data[px][py][2] - straight[px][py][2];
        }
    }
    double average_diff = diff / (w*h);
    REQUIRE(average_diff < 10);    
}

TEST_CASE("Test Sphere Left", "[Test Image]")
{
    Sphere sphere(2, {-5,0,0}, Material(255, 255, 0));
    int w = 100; 
    int h = 100;
    Camera cam(w, h);
    auto image_data = Render::genImage(cam, sphere);
    auto straight = loadImage("data/left.pbm", w, h);

    double diff;
    for(int px = 0; px < w; px++)
    {
        for(int py = 0; py < h; py++)
        {
            diff += image_data[px][py][0] - straight[px][py][0];
            diff += image_data[px][py][1] - straight[px][py][1];
            diff += image_data[px][py][2] - straight[px][py][2];
        }
    }
    double average_diff = diff / (w*h);
    REQUIRE(average_diff < 10);    
}

TEST_CASE("Test Sphere Right", "[Test Image]")
{
    Sphere sphere(2, {5,0,0}, Material(0, 255, 0));
    int w = 100; 
    int h = 100;
    Camera cam(w, h);
    auto image_data = Render::genImage(cam, sphere);
    auto straight = loadImage("data/right.pbm", w, h);

    double diff;
    for(int px = 0; px < w; px++)
    {
        for(int py = 0; py < h; py++)
        {
            diff += image_data[px][py][0] - straight[px][py][0];
            diff += image_data[px][py][1] - straight[px][py][1];
            diff += image_data[px][py][2] - straight[px][py][2];
        }
    }
    double average_diff = diff / (w*h);
    REQUIRE(average_diff < 10);    
}

TEST_CASE("Test Sphere Top Right", "[Test Image]")
{
    Sphere sphere(2, {5,5,0}, Material(0, 255, 255));
    int w = 100; 
    int h = 100;
    Camera cam(w, h);
    auto image_data = Render::genImage(cam, sphere);
    auto straight = loadImage("data/topright.pbm", w, h);

    double diff;
    for(int px = 0; px < w; px++)
    {
        for(int py = 0; py < h; py++)
        {
            diff += image_data[px][py][0] - straight[px][py][0];
            diff += image_data[px][py][1] - straight[px][py][1];
            diff += image_data[px][py][2] - straight[px][py][2];
        }
    }
    double average_diff = diff / (w*h);
    REQUIRE(average_diff < 10);    
}

TEST_CASE("Test Camera Position", "[Test Image]")
{
    Sphere sphere(2, {0,0,0}, Material(0, 255, 255));
    int w = 100; 
    int h = 100;
    Camera cam(w, h, {2, 2, 10});
    auto image_data = Render::genImage(cam, sphere);
    auto straight = loadImage("data/movecam.pbm", w, h);

    double diff;
    for(int px = 0; px < w; px++)
    {
        for(int py = 0; py < h; py++)
        {
            diff += image_data[px][py][0] - straight[px][py][0];
            diff += image_data[px][py][1] - straight[px][py][1];
            diff += image_data[px][py][2] - straight[px][py][2];
        }
    }
    double average_diff = diff / (w*h);
    REQUIRE(average_diff < 10);    
}

TEST_CASE("Test Aspect Ratio", "[Test Image]")
{
    Sphere sphere(5, {0,0,0}, Material(0, 0, 255));
    int w = 200; 
    int h = 100;
    Camera cam(w, h);
    auto image_data = Render::genImage(cam, sphere);
    auto straight = loadImage("data/aspect.pbm", w, h);

    double diff;
    for(int px = 0; px < w; px++)
    {
        for(int py = 0; py < h; py++)
        {
            diff += image_data[px][py][0] - straight[px][py][0];
            diff += image_data[px][py][1] - straight[px][py][1];
            diff += image_data[px][py][2] - straight[px][py][2];
        }
    }
    double average_diff = std::abs(diff / (w*h));
    REQUIRE(average_diff < 10);    
}
