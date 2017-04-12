// Copyright 2016 Yahoo Inc. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.
#include <vespa/fastos/fastos.h>
#include <vespa/log/log.h>
LOG_SETUP("reprocessing_runner_test");

#include <vespa/searchcore/proton/reprocessing/i_reprocessing_task.h>
#include <vespa/searchcore/proton/reprocessing/reprocessingrunner.h>
#include <vespa/vespalib/testkit/testapp.h>

using namespace proton;

struct Fixture
{
    ReprocessingRunner _runner;
    Fixture()
        : _runner()
    {
    }
};

typedef ReprocessingRunner::ReprocessingTasks TaskList;

struct MyTask : public IReprocessingTask
{
    ReprocessingRunner &_runner;
    double _initProgress;
    double _middleProgress;
    double _finalProgress;
    double _myProgress;
    double _weight;

    MyTask(ReprocessingRunner &runner,
           double initProgress,
           double middleProgress,
           double finalProgress,
           double weight)
        : _runner(runner),
          _initProgress(initProgress),
          _middleProgress(middleProgress),
          _finalProgress(finalProgress),
          _myProgress(0.0),
          _weight(weight)
    {
    }

    virtual void
    run() override
    {
        ASSERT_EQUAL(_initProgress, _runner.getProgress());
        _myProgress = 0.5;
        ASSERT_EQUAL(_middleProgress, _runner.getProgress());
        _myProgress = 1.0;
        ASSERT_EQUAL(_finalProgress, _runner.getProgress());
    }

    virtual Progress
    getProgress(void) const override
    {
        return Progress(_myProgress, _weight);
    }

    static std::shared_ptr<MyTask>
    create(ReprocessingRunner &runner,
           double initProgress,
           double middleProgress,
           double finalProgress,
           double weight)
    {
        return std::make_shared<MyTask>(runner,
                                        initProgress,
                                        middleProgress,
                                        finalProgress,
                                        weight);
    }
};

TEST_F("require that progress is calculated when tasks are executed", Fixture)
{
    TaskList tasks;
    EXPECT_EQUAL(0.0, f._runner.getProgress());
    tasks.push_back(MyTask::create(f._runner,
                                   0.0,
                                   0.1,
                                   0.2,
                                   1.0));
    tasks.push_back(MyTask::create(f._runner,
                                   0.2,
                                   0.6,
                                   1.0,
                                   4.0));
    f._runner.addTasks(tasks);
    tasks.clear();
    EXPECT_EQUAL(0.0, f._runner.getProgress());
    f._runner.run();
    EXPECT_EQUAL(1.0, f._runner.getProgress());
}


TEST_F("require that runner can be reset", Fixture)
{
    TaskList tasks;
    EXPECT_EQUAL(0.0, f._runner.getProgress());
    tasks.push_back(MyTask::create(f._runner,
                                   0.0,
                                   0.5,
                                   1.0,
                                   1.0));
    f._runner.addTasks(tasks);
    tasks.clear();
    EXPECT_EQUAL(0.0, f._runner.getProgress());
    f._runner.run();
    EXPECT_EQUAL(1.0, f._runner.getProgress());
    f._runner.reset();
    EXPECT_EQUAL(0.0, f._runner.getProgress());
    tasks.push_back(MyTask::create(f._runner,
                                   0.0,
                                   0.5,
                                   1.0,
                                   1.0));
    f._runner.addTasks(tasks);
    tasks.clear();
    EXPECT_EQUAL(0.0, f._runner.getProgress());
    f._runner.reset();
    EXPECT_EQUAL(0.0, f._runner.getProgress());
    tasks.push_back(MyTask::create(f._runner,
                                   0.0,
                                   0.5,
                                   1.0,
                                   4.0));
    f._runner.addTasks(tasks);
    tasks.clear();
    EXPECT_EQUAL(0.0, f._runner.getProgress());
    f._runner.run();
    EXPECT_EQUAL(1.0, f._runner.getProgress());
}


TEST_MAIN()
{
    TEST_RUN_ALL();
}
