#pragma once

#include "SampleBase.hpp"

namespace Diligent
{

    class FractalViewer final : public SampleBase
    {
    public:
        void Initialize(const SampleInitInfo& InitInfo) override final;
        void Render() override final;
        void Update(double CurrTime, double ElapsedTime, bool DoUpdateUI) override final;

        const Char* GetSampleName() const override final { return "Fractal Viewer"; }

    private:
        // Aquí luego podrás declarar tu PSO, constantes, buffers, etc.
    };

} // namespace Diligent
