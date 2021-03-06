#pragma once

#include "gmLogicItkImageToImageFilter.hpp"

#include <itkBinomialBlurImageFilter.h>

namespace gm
{
    namespace Logic
    {
        namespace Itk
        {
            class BinomialBlurImageFilter : public ImageToImageFilter
            {
            public:
                struct ID
                {
                    static constexpr auto input = "input";
                    static constexpr auto repititions = "repititions";
                    static constexpr auto output = "output";
                };
                
                static Register<BinomialBlurImageFilter> Register;

                using Access = Enable::ImageTypes <
                    Enable::Dimension<2,3>,
                    Enable::Scalar<unsigned char, unsigned short, unsigned int, char, short, int, float, double>
                >;

                BinomialBlurImageFilter() : ImageToImageFilter("BinomialBlurImageFilter")
                {
                    log_trace(Log::New);
                    
                    this->addParamGroup("BinomialBlurImageFilter");
                    
                    this->add(new Slot::Input<Data::Image>(ID::input, Access::MakeConstraints(), Data::Required));
                    this->add(new Param::Number(ID::repititions, 1));
                    this->add(new Slot::Output<Data::Image>(ID::output, Access::MakeConstraints()));
                }
                
                template <class TImage>
                auto process() -> void
                {
                    using ImageType     = typename TImage::itk_image_t;
                    using FilterType    = itk::BinomialBlurImageFilter<ImageType, ImageType>;
                    
                    auto filter         = FilterType::New();
                    auto output         = this->getOutput<Data::Image>(ID::output);
                    
                    applyParameters<FilterType>(filter);
                    
                    filter->SetRepetitions(this->getParam<Param::Number>(ID::repititions)->getValue());
                    
                    for (auto it : this->inputIterator())
                    {
                        filter->SetInput(Image::ToItk<ImageType>(it.image()));
                        filter->Update();
                        output->addImage(Image::FromItk<ImageType>(filter->GetOutput()));
                    }
                }
                
                auto run() -> void override
                {
                    log_trace(Log::Func);
                    
                    Invoke(this);
                }
            };
        }
    }
}