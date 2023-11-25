
#include <QGuiApplication>
#include <QImage>
#include <QFile>
#include <rhi/qrhi.h>


int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

#if QT_CONFIG(vulkan)
    QVulkanInstance inst;
#endif
    std::unique_ptr<QRhi> rhi;
#if defined(Q_OS_WIN)
    QRhiD3D12InitParams params;
    rhi.reset(QRhi::create(QRhi::D3D12, &params));
#elif defined(Q_OS_MACOS) || defined(Q_OS_IOS)
    QRhiMetalInitParams params;
    rhi.reset(QRhi::create(QRhi::Metal, &params, {QRhi::DebugMarkers}));
#elif QT_CONFIG(vulkan)
    inst.setExtensions(QRhiVulkanInitParams::preferredInstanceExtensions());
    if (inst.create()) {
        QRhiVulkanInitParams params;
        params.inst = &inst;
        rhi.reset(QRhi::create(QRhi::Vulkan, &params));
    } else {
        qFatal("Failed to create Vulkan instance");
    }
#endif
    if (rhi) {
        qDebug() << rhi->backendName() << rhi->driverInfo();
        qDebug() << "compute supported:" << rhi->isFeatureSupported(QRhi::Compute);
        qDebug() << "read non uniform buffer:" << rhi->isFeatureSupported(QRhi::ReadBackNonUniformBuffer);
    } else {
        qFatal("Failed to initialize RHI");
    }

    // Assuming you have a QRhi instance, a QRhiCommandBuffer, and a running QCoreApplication
    const int numElements = 10;
    const int bufferSize = numElements * sizeof(float); // Size of the buffer in bytes
    float dataA[numElements] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    float dataB[numElements] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    float bufferResultData[numElements] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

    QRhiBuffer *bufferA = rhi->newBuffer(QRhiBuffer::Type::Static, QRhiBuffer::UsageFlag::StorageBuffer, bufferSize);
    bufferA->create();

    QRhiBuffer *bufferB = rhi->newBuffer(QRhiBuffer::Type::Static, QRhiBuffer::UsageFlag::StorageBuffer, bufferSize);
    bufferB->create();

    // Assume bufferResult is already created similarly to bufferA and bufferB
    QRhiBuffer *bufferResult = rhi->newBuffer(QRhiBuffer::Type::Static, QRhiBuffer::UsageFlag::StorageBuffer, bufferSize);
    bufferResult->create();

    // Create a resource update batch
    QRhiResourceUpdateBatch *resourceUpdateBatch = rhi->nextResourceUpdateBatch();

    // Update bufferA with data
    int offset = 0;
    resourceUpdateBatch->uploadStaticBuffer(bufferA, offset, bufferSize, dataA);
    resourceUpdateBatch->uploadStaticBuffer(bufferB, offset, bufferSize, dataB);
    resourceUpdateBatch->uploadStaticBuffer(bufferResult, offset, bufferSize, bufferResultData);

    QRhiComputePipeline *computePipeline = rhi->newComputePipeline();


    // Set up shader resource bindings
    QRhiShaderResourceBindings *srb = rhi->newShaderResourceBindings();
    srb->setBindings({
        QRhiShaderResourceBinding::bufferLoad(0, QRhiShaderResourceBinding::ComputeStage, bufferA),
        QRhiShaderResourceBinding::bufferLoad(1, QRhiShaderResourceBinding::ComputeStage, bufferB),
        QRhiShaderResourceBinding::bufferStore(2, QRhiShaderResourceBinding::ComputeStage, bufferResult)
    });
    srb->create();

    // Load the compiled shader
    QFile shaderFile(":/arrayAdd.comp.qsb");
    if (!shaderFile.open(QIODevice::ReadOnly)) {
        qWarning("Failed to load compute shader!");
        return 1;
    }

    QByteArray shaderData = shaderFile.readAll();

    // Create the shader stage
    QShader computeShader = QShader::fromSerialized(shaderData);
    qDebug() << "Shader valid:" << computeShader.isValid();

    QRhiShaderStage computeStage(QRhiShaderStage::Compute, computeShader);

    // Set up the compute pipeline
    computePipeline->setShaderStage(computeStage);
    computePipeline->setShaderResourceBindings(srb);
    computePipeline->create();

    // Run the compute shader
    QRhiCommandBuffer *cmdBuffer; // Assume this is already set up
    rhi->beginOffscreenFrame(&cmdBuffer);

    cmdBuffer->beginComputePass(resourceUpdateBatch); //Make sure you push the resource you're going to use into the batch
    cmdBuffer->setComputePipeline(computePipeline);
    cmdBuffer->setShaderResources();
    cmdBuffer->dispatch(numElements, 1, 1); // Adjust dispatch size based on your needs

    QRhiReadbackResult readBackResult;
    readBackResult.completed = [&readBackResult]() {
        // This lambda is called when the read-back is completed
        if (!readBackResult.data.isEmpty()) {
            const float *resultData = reinterpret_cast<const float *>(readBackResult.data.constData());
            for (int i = 0; i < bufferSize / sizeof(float); ++i) {
                qDebug() << "i:" << i << "value:" << resultData[i];
            }
        } else {
            qWarning("Failed to read back data");
        }
    };
    resourceUpdateBatch = rhi->nextResourceUpdateBatch();
    resourceUpdateBatch->readBackBuffer(bufferResult, 0, bufferSize, &readBackResult);

    qDebug() << "endComputePass";
    cmdBuffer->endComputePass(resourceUpdateBatch);

    // Submit the command buffer for execution

    qDebug() << "endOffscreenFrame";
    rhi->endOffscreenFrame(); //This is where readback happen

    qDebug() << "finish";
    rhi->finish();



//    // This lambda is called when the read-back is completed
//    if (!readBackResult.data.isEmpty()) {
//        const float *resultData = reinterpret_cast<const float *>(readBackResult.data.constData());
//        for (int i = 0; i < bufferSize / sizeof(float); ++i) {
//            qDebug() << "i:" << i << "value:" << resultData[i];
//        }
//    } else {
//        qWarning("Failed to read back data");
//    }

    qDebug() << "Finised with frame!";

    a.exec();
    return 0;
}
