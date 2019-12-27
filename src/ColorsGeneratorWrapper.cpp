#include "ColorsGeneratorWrapper.h"

namespace colors
{
    ColorGeneratorWrapper::ColorGeneratorWrapper()
        : generator_{std::make_shared<ConstantColor>()}
    {
        std::cout << "empty ctor wrapper\n";
    }

    ColorGeneratorWrapper::ColorGeneratorWrapper(std::shared_ptr<ColorGenerator> gen)
        : generator_{gen}
    {
        std::cout << "complete ctor wrapper\n";
        std::cout << "poll " << poll_modification() << "\n";
    }

    ColorGeneratorWrapper::ColorGeneratorWrapper(const ColorGeneratorWrapper& other)
        : generator_{other.generator_->clone()}
    {
        std::cout << "copy ctor wrapper\n";
    }
    ColorGeneratorWrapper::ColorGeneratorWrapper(ColorGeneratorWrapper&& other)
        : generator_{std::move(other.generator_)}
    {
        std::cout << "move ctor wrapper\n";
        std::cout << "poll " << poll_modification() << "\n";
        other.generator_.reset();
    }
    ColorGeneratorWrapper& ColorGeneratorWrapper::operator=(const ColorGeneratorWrapper& other)
    {
        std::cout << "cpy assign wrapper\n";
        if (this != &other)
        {
            generator_ = other.generator_->clone();
        }
        return *this;
    }
    ColorGeneratorWrapper& ColorGeneratorWrapper::operator=(ColorGeneratorWrapper&& other)
    {
        std::cout << "move assign wrapper\n";
        if (this != &other)
        {
            generator_ = std::move(other.generator_);
            other.generator_.reset();
        }
        return *this;
    }

    std::shared_ptr<ColorGenerator> ColorGeneratorWrapper::unwrap() const
    {
        return generator_;
    }

    void ColorGeneratorWrapper::wrap(std::shared_ptr<ColorGenerator> gen)
    {
        generator_ = gen;
        indicate_modification();
    }

    bool ColorGeneratorWrapper::poll_modification()
    {
        return Indicator::poll_modification() | generator_->poll_modification();
    }
}
