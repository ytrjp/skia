/*
 * Copyright 2018 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrConstColorProcessor.fp; do not modify.
 **************************************************************************************************/
#include "GrConstColorProcessor.h"

#include "src/core/SkUtils.h"
#include "src/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLConstColorProcessor : public GrGLSLFragmentProcessor {
public:
    GrGLSLConstColorProcessor() {}
    void emitCode(EmitArgs& args) override {
        GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
        const GrConstColorProcessor& _outer = args.fFp.cast<GrConstColorProcessor>();
        (void)_outer;
        auto color = _outer.color;
        (void)color;
        colorVar = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag,
                                                    kHalf4_GrSLType, "color");
        fragBuilder->codeAppendf(
                R"SkSL(%s = %s;
)SkSL",
                args.fOutputColor, args.fUniformHandler->getUniformCStr(colorVar));
    }

private:
    void onSetData(const GrGLSLProgramDataManager& pdman,
                   const GrFragmentProcessor& _proc) override {
        const GrConstColorProcessor& _outer = _proc.cast<GrConstColorProcessor>();
        {
            const SkPMColor4f& colorValue = _outer.color;
            if (colorPrev != colorValue) {
                colorPrev = colorValue;
                pdman.set4fv(colorVar, 1, colorValue.vec());
            }
        }
    }
    SkPMColor4f colorPrev = {SK_FloatNaN, SK_FloatNaN, SK_FloatNaN, SK_FloatNaN};
    UniformHandle colorVar;
};
GrGLSLFragmentProcessor* GrConstColorProcessor::onCreateGLSLInstance() const {
    return new GrGLSLConstColorProcessor();
}
void GrConstColorProcessor::onGetGLSLProcessorKey(const GrShaderCaps& caps,
                                                  GrProcessorKeyBuilder* b) const {}
bool GrConstColorProcessor::onIsEqual(const GrFragmentProcessor& other) const {
    const GrConstColorProcessor& that = other.cast<GrConstColorProcessor>();
    (void)that;
    if (color != that.color) return false;
    return true;
}
GrConstColorProcessor::GrConstColorProcessor(const GrConstColorProcessor& src)
        : INHERITED(kGrConstColorProcessor_ClassID, src.optimizationFlags()), color(src.color) {
    this->cloneAndRegisterAllChildProcessors(src);
}
std::unique_ptr<GrFragmentProcessor> GrConstColorProcessor::clone() const {
    return std::make_unique<GrConstColorProcessor>(*this);
}
#if GR_TEST_UTILS
SkString GrConstColorProcessor::onDumpInfo() const {
    return SkStringPrintf("(color=half4(%f, %f, %f, %f))", color.fR, color.fG, color.fB, color.fA);
}
#endif
GR_DEFINE_FRAGMENT_PROCESSOR_TEST(GrConstColorProcessor);
#if GR_TEST_UTILS
std::unique_ptr<GrFragmentProcessor> GrConstColorProcessor::TestCreate(GrProcessorTestData* d) {
    SkPMColor4f color;
    int colorPicker = d->fRandom->nextULessThan(3);
    switch (colorPicker) {
        case 0: {
            uint32_t a = d->fRandom->nextULessThan(0x100);
            uint32_t r = d->fRandom->nextULessThan(a + 1);
            uint32_t g = d->fRandom->nextULessThan(a + 1);
            uint32_t b = d->fRandom->nextULessThan(a + 1);
            color = SkPMColor4f::FromBytes_RGBA(GrColorPackRGBA(r, g, b, a));
            break;
        }
        case 1:
            color = SK_PMColor4fTRANSPARENT;
            break;
        case 2:
            uint32_t c = d->fRandom->nextULessThan(0x100);
            color = SkPMColor4f::FromBytes_RGBA(c | (c << 8) | (c << 16) | (c << 24));
            break;
    }
    return GrConstColorProcessor::Make(color);
}
#endif
