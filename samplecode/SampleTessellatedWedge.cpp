/*
 * Copyright 2019 Google LLC.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "include/core/SkCanvas.h"
#include "samplecode/Sample.h"
#include "src/core/SkPathPriv.h"
#include "tools/ToolUtils.h"

#if SK_SUPPORT_GPU

#include "include/gpu/GrContext.h"
#include "src/gpu/GrClip.h"
#include "src/gpu/GrContextPriv.h"
#include "src/gpu/GrMemoryPool.h"
#include "src/gpu/GrRenderTargetContext.h"
#include "src/gpu/GrRenderTargetContextPriv.h"
#include "src/gpu/tessellate/GrTessellatePathOp.h"

// This sample enables wireframe and visualizes the triangulation generated by
// GrTessellateWedgeShader.
class TessellatedWedgeView : public Sample {
public:
    TessellatedWedgeView() {
        fPath.moveTo(100, 200);
        fPath.cubicTo(100, 100, 400, 100, 400, 200);
        fPath.lineTo(250, 500);
    }

private:
    void onDrawContent(SkCanvas*) override;
    Sample::Click* onFindClickHandler(SkScalar x, SkScalar y, skui::ModifierKey) override;
    bool onClick(Sample::Click*) override;
    bool onChar(SkUnichar) override;

    SkString name() override { return SkString("TessellatedWedge"); }

    SkMatrix fLastViewMatrix = SkMatrix::I();
    SkPath fPath;
    GrTessellatePathOp::Flags fFlags = GrTessellatePathOp::Flags::kWireframe;

    class Click;
};

void TessellatedWedgeView::onDrawContent(SkCanvas* canvas) {
    canvas->clear(SK_ColorBLACK);

    GrContext* ctx = canvas->getGrContext();
    GrRenderTargetContext* rtc = canvas->internal_private_accessTopLayerRenderTargetContext();

    SkString error;
    if (!rtc || !ctx) {
        error = "GPU Only.";
    } else if (!ctx->priv().caps()->shaderCaps()->tessellationSupport()) {
        error = "GPU tessellation not supported.";
    } else if (1 == rtc->numSamples() && !ctx->priv().caps()->mixedSamplesSupport()) {
        error = "MSAA/mixed samples only.";
    }
    if (!error.isEmpty()) {
        SkFont font(nullptr, 20);
        SkPaint captionPaint;
        captionPaint.setColor(SK_ColorWHITE);
        canvas->drawString(error.c_str(), 10, 30, font, captionPaint);
        return;
    }

    GrPaint paint;
    paint.setColor4f({1,0,1,1});

    GrAAType aa;
    if (rtc->numSamples() > 1) {
        aa = GrAAType::kMSAA;
    } else if (rtc->asRenderTargetProxy()->canUseMixedSamples(*ctx->priv().caps())) {
        aa = GrAAType::kCoverage;
    } else {
        aa = GrAAType::kNone;
    }

    GrOpMemoryPool* pool = ctx->priv().opMemoryPool();
    rtc->priv().testingOnly_addDrawOp(pool->allocate<GrTessellatePathOp>(
            canvas->getTotalMatrix(), fPath, std::move(paint), aa, fFlags));

    // Draw the path points.
    SkPaint pointsPaint;
    pointsPaint.setColor(SK_ColorBLUE);
    pointsPaint.setStrokeWidth(8);
    canvas->drawPoints(SkCanvas::kPoints_PointMode, fPath.countPoints(),
                       SkPathPriv::PointData(fPath), pointsPaint);

    fLastViewMatrix = canvas->getTotalMatrix();
}

class TessellatedWedgeView::Click : public Sample::Click {
public:
    Click(int ptIdx) : fPtIdx(ptIdx) {}

    void doClick(SkPath* path) {
        if (fPtIdx >= 0) {
            SkPoint pt = path->getPoint(fPtIdx);
            ToolUtils::set_path_pt(fPtIdx, pt + fCurr - fPrev, path);
        } else {
            path->transform(
                    SkMatrix::MakeTrans(fCurr.x() - fPrev.x(), fCurr.y() - fPrev.y()), path);
        }
    }

private:
    int fPtIdx;
};

Sample::Click* TessellatedWedgeView::onFindClickHandler(SkScalar x, SkScalar y, skui::ModifierKey) {
    const SkPoint* pts = SkPathPriv::PointData(fPath);
    float fuzz = 20 / fLastViewMatrix.getMaxScale();
    for (int i = 0; i < fPath.countPoints(); ++i) {
        SkPoint screenPoint = pts[i];
        if (fabs(x - screenPoint.x()) < fuzz && fabsf(y - screenPoint.y()) < fuzz) {
            return new Click(i);
        }
    }
    return new Click(-1);
}

bool TessellatedWedgeView::onClick(Sample::Click* click) {
    Click* myClick = (Click*)click;
    myClick->doClick(&fPath);
    return true;
}

bool TessellatedWedgeView::onChar(SkUnichar unichar) {
    switch (unichar) {
        case 'w':
            fFlags = (GrTessellatePathOp::Flags)(
                    (int)fFlags ^ (int)GrTessellatePathOp::Flags::kWireframe);
            return true;
        case 'D': {
            fPath.dump();
            return true;
        }
    }
    return false;
}

DEF_SAMPLE(return new TessellatedWedgeView;)

#endif  // SK_SUPPORT_GPU
