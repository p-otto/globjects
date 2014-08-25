#pragma once

#include <globjects-base/ref_ptr.h>

#include <globjects/Buffer.h>
#include <globjects/FrameBufferObject.h>
#include <globjects/Program.h>
#include <globjects/Texture.h>
#include <globjects/TransformFeedback.h>
#include <globjects/VertexArrayObject.h>

#include <globjects-utils/ScreenAlignedQuad.h>

#include "AbstractParticleTechnique.h"

class TransformFeedbackParticles : public AbstractParticleTechnique
{
public:
    TransformFeedbackParticles(
        const std::vector<glm::vec4> & positions
    ,   const std::vector<glm::vec4> & velocities
    ,   const glo::Texture & forces
    ,   const gloutils::Camera & camera);
    virtual ~TransformFeedbackParticles();

    virtual void initialize() override;
    virtual void reset() override;

    virtual void step(float elapsed) override;
    virtual void draw(float elapsed) override;

    virtual void resize() override;

protected:
    glo::ref_ptr<glo::TransformFeedback> m_transformFeedback;
    glo::ref_ptr<glo::Program> m_transformFeedbackProgram;

    glo::ref_ptr<glo::Buffer> m_sourcePositions;
    glo::ref_ptr<glo::Buffer> m_sourceVelocities;
    glo::ref_ptr<glo::Buffer> m_targetPositions;
    glo::ref_ptr<glo::Buffer> m_targetVelocities;

    glo::ref_ptr<glo::Program> m_drawProgram;

    glo::ref_ptr<glo::VertexArrayObject> m_vao;

    glo::ref_ptr<glo::FrameBufferObject> m_fbo;
    glo::ref_ptr<glo::Texture> m_color;

    glo::ref_ptr<gloutils::ScreenAlignedQuad> m_quad;
    glo::ref_ptr<gloutils::ScreenAlignedQuad> m_clear;
};
