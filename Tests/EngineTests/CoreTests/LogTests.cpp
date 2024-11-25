// LogTests.cpp
// AI-generated unit tests for the RZLog class
#include <Razix/Core/Log/RZLog.h> 
#include <gtest/gtest.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace Razix {
    namespace Debug {

        class MockSink : public spdlog::sinks::base_sink<std::mutex>
        {
        public:
            struct LogMessage
            {
                spdlog::level::level_enum level;
                std::string               message;
            };

            const std::vector<LogMessage>& getMessages() const { return messages_; }
            void                           clear() { messages_.clear(); }

        protected:
            void sink_it_(const spdlog::details::log_msg& msg) override
            {
                spdlog::memory_buf_t formatted;
                spdlog::sinks::base_sink<std::mutex>::formatter_->format(msg, formatted);
                messages_.push_back({msg.level, std::string(formatted.data(), formatted.size())});
            }

            void flush_() override {}

        private:
            std::vector<LogMessage> messages_;
        };

        static void ClearAndSetPattern()
        {
            auto coreLogger = Razix::Debug::RZLog::GetCoreLogger();
            auto appLogger  = Razix::Debug::RZLog::GetApplicationLogger();

            // Define the new pattern (no timestamps, clean format)
            const std::string corePattern = "%^[Razix Core] [%l]: %v %$";
            const std::string appPattern  = "%^[Razix App] [%l]: %v %$";

            // Apply the pattern to all sinks of the core logger
            if (coreLogger) {
                for (auto& sink: coreLogger->sinks()) {
                    sink->set_pattern(corePattern);
                }
            }

            // Apply the pattern to all sinks of the application logger
            if (appLogger) {
                for (auto& sink: appLogger->sinks()) {
                    sink->set_pattern(appPattern);
                }
            }
        }

        class RZLogTests : public ::testing::Test
        {
        protected:
            void SetUp() override
            {
                // Setup before each test case, for example initializing the logging system
                RZLog::StartUp();
                ClearAndSetPattern();
            }

            void TearDown() override
            {
                // Cleanup after each test case
                RZLog::Shutdown();
            }
        };

        TEST_F(RZLogTests, LogMessageOrder)
        {
            auto mockSink = std::make_shared<MockSink>();
            RZLog::RegisterCoreLoggerSink(mockSink);
            ClearAndSetPattern();

            RAZIX_CORE_INFO("First Message");
            RAZIX_CORE_WARN("Second Message");
            RAZIX_CORE_ERROR("Third Message");

            const auto& messages = mockSink->getMessages();
            ASSERT_EQ(messages.size(), 3) << "There should be 3 log messages.";
            EXPECT_EQ(messages[0].message, "[Razix Core] [info]: First Message \n") << "First message should be logged first.";
            EXPECT_EQ(messages[1].message, "[Razix Core] [warning]: Second Message \n") << "Second message should be logged second.";
            EXPECT_EQ(messages[2].message, "[Razix Core] [error]: Third Message \n") << "Third message should be logged third.";
        }

        TEST_F(RZLogTests, RegisterMultipleSinks)
        {
            auto mockSink1 = std::make_shared<MockSink>();
            auto mockSink2 = std::make_shared<MockSink>();
            RZLog::RegisterCoreLoggerSink(mockSink1);
            RZLog::RegisterCoreLoggerSink(mockSink2);
            ClearAndSetPattern();

            RAZIX_CORE_INFO("Message for multiple sinks");
            // 2 since we have default onces from startup call
            EXPECT_EQ(mockSink1->getMessages().size(), 2) << "First sink should receive the log message.";
            EXPECT_EQ(mockSink2->getMessages().size(), 1) << "Second sink should also receive the log message.";
            EXPECT_EQ(mockSink1->getMessages()[0].message, "[Razix Core] [info]: Message for multiple sinks \n");
            EXPECT_EQ(mockSink2->getMessages()[0].message, "[Razix Core] [info]: Message for multiple sinks \n");
        }

        TEST_F(RZLogTests, LoggingConcurrency)
        {
            auto mockSink = std::make_shared<MockSink>();
            RZLog::RegisterCoreLoggerSink(mockSink);
            ClearAndSetPattern();

            constexpr int threadCount       = 10;
            constexpr int messagesPerThread = 100;

            auto logTask = [messagesPerThread, threadCount]() {
                for (int i = 0; i < messagesPerThread; ++i) {
                    RAZIX_CORE_INFO("Concurrent Log Message");
                }
            };

            std::vector<std::thread> threads;
            for (int i = 0; i < threadCount; ++i) {
                threads.emplace_back(logTask);
            }

            for (auto& thread: threads) {
                thread.join();
            }

            const auto& messages = mockSink->getMessages();
            EXPECT_EQ(messages.size(), threadCount * messagesPerThread)
                << "All threads' messages should be logged correctly.";
        }

        TEST_F(RZLogTests, MessageTruncation)
        {
            auto mockSink = std::make_shared<MockSink>();
            RZLog::RegisterCoreLoggerSink(mockSink);
            ClearAndSetPattern();

            std::string longMessage(1024, 'A');    // Create a 1024-character-long message
            RAZIX_CORE_INFO(longMessage);

            const auto& messages = mockSink->getMessages();
            ASSERT_EQ(messages.size(), 1) << "One message should be logged.";
            EXPECT_EQ(messages[0].message, "[Razix Core] [info]: " + longMessage + " \n")
                << "[Razix Core] [info]: Long message should not be truncated.";
        }

        TEST_F(RZLogTests, InvalidSinkRegistration)
        {
            EXPECT_NO_THROW(RZLog::RegisterCoreLoggerSink(nullptr))
                << "Registering a nullptr sink should not crash the system.";
        }

        TEST_F(RZLogTests, EmptyMessageLogging)
        {
            auto mockSink = std::make_shared<MockSink>();
            RZLog::RegisterCoreLoggerSink(mockSink);
            ClearAndSetPattern();

            RAZIX_CORE_INFO("");    // Log an empty message

            const auto& messages = mockSink->getMessages();
            ASSERT_EQ(messages.size(), 1) << "An empty message should still be logged.";
            EXPECT_EQ(messages[0].message, "[Razix Core] [info]:  \n") << "Empty message should be logged as a newline.";
        }

        TEST_F(RZLogTests, LogLevelFiltering)
        {
            auto mockSink = std::make_shared<MockSink>();
            RZLog::RegisterCoreLoggerSink(mockSink);
            ClearAndSetPattern();

            auto coreLogger = RZLog::GetCoreLogger();
            coreLogger->set_level(spdlog::level::warn);    // Only log warnings or higher

            RAZIX_CORE_INFO("This should not be logged");
            RAZIX_CORE_WARN("This should be logged");

            const auto& messages = mockSink->getMessages();
            ASSERT_EQ(messages.size(), 1) << "Only warnings or higher should be logged.";
            EXPECT_EQ(messages[0].message, "[Razix Core] [warning]: This should be logged \n")
                << "Only the warning message should appear.";
        }

        TEST_F(RZLogTests, LoggingAfterShutdown)
        {
            auto mockSink = std::make_shared<MockSink>();
            RZLog::RegisterCoreLoggerSink(mockSink);
            ClearAndSetPattern();

            RZLog::Shutdown();    // Shutdown the logging system

            const auto& messages = mockSink->getMessages();
            EXPECT_EQ(messages.size(), 1) << "No messages should be logged after shutdown.";
        }

        // Test that logging doesn't occur when in distribution mode
        TEST_F(RZLogTests, DistributionModeLogging)
        {
#ifdef RAZIX_DISTRIBUTION
            auto mockSink = std::make_shared<MockSink>();
            RZLog::RegisterCoreLoggerSink(mockSink);
            RZLog::RegisterApplicationLoggerSink(mockSink);
            ClearAndSetPattern();

            // Attempt to log messages
            RAZIX_CORE_TRACE("Core Trace Message");
            RAZIX_CORE_INFO("Core Info Message");
            RAZIX_CORE_WARN("Core Warn Message");
            RAZIX_CORE_ERROR("Core Error Message");

            RAZIX_TRACE("App Trace Message");
            RAZIX_INFO("App Info Message");
            RAZIX_WARN("App Warn Message");
            RAZIX_ERROR("App Error Message");

            const auto& messages = mockSink->getMessages();
            EXPECT_EQ(messages.size(), 0) << "No messages should be logged in distribution mode.";
#else
            GTEST_SKIP() << "Test is only valid when RAZIX_DISTRIBUTION is defined.";
#endif
        }

    }    // namespace Debug
}    // namespace Razix
