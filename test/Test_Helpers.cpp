#include <protobuf_parser/helpers.hpp>
#include <protobuf/message.pb.h>

#include <gtest/gtest.h>

TEST(ParseDelimited, DefaultTest)
{
  std::shared_ptr<TestTask::Messages::WrapperMessage> delimited;

  TestTask::Messages::WrapperMessage message;
  message.mutable_request_for_fast_response();

  auto buffer = serializeDelimited(message);
  size_t bytesConsumed = 0;

  delimited = parseDelimited<TestTask::Messages::WrapperMessage>(
                buffer->data(),
                buffer->size(),
                &bytesConsumed
              );

  ASSERT_FALSE(delimited == nullptr);
  EXPECT_TRUE(delimited->has_request_for_fast_response());
  EXPECT_EQ(bytesConsumed, buffer->size());
}

TEST(ParseDelimited, TimeFormatTest)
{
  std::shared_ptr<TestTask::Messages::WrapperMessage> delimited;

  TestTask::Messages::WrapperMessage message;
  message.mutable_fast_response()->set_current_date_time("20240701T120000.001");

  auto buffer = serializeDelimited(message);
  size_t bytesConsumed = 0;

  delimited = parseDelimited<TestTask::Messages::WrapperMessage>(
                buffer->data(),
                buffer->size(),
                &bytesConsumed
              );

  ASSERT_FALSE(delimited == nullptr);
  EXPECT_TRUE(delimited->has_fast_response());
  EXPECT_EQ(bytesConsumed, buffer->size());
  EXPECT_EQ(delimited->fast_response().current_date_time(), "20240701T120000.001");
}

TEST(ParseDelimited, TwoMessagesTest)
{
  std::shared_ptr<TestTask::Messages::WrapperMessage> delimited;

  TestTask::Messages::WrapperMessage message1;
  message1.mutable_request_for_fast_response();

  TestTask::Messages::WrapperMessage message2;
  message2.mutable_slow_response()->set_connected_client_count(10);

  auto buffer1 = serializeDelimited(message1);
  auto buffer2 = serializeDelimited(message2);

  std::vector<char> combinedBuffer(buffer1->begin(), buffer1->end());
  combinedBuffer.insert(combinedBuffer.end(), buffer2->begin(), buffer2->end());

  size_t bytesConsumed = 0;
  delimited = parseDelimited<TestTask::Messages::WrapperMessage>(
                combinedBuffer.data(),
                combinedBuffer.size(),
                &bytesConsumed
              );

  ASSERT_FALSE(delimited == nullptr);
  EXPECT_TRUE(delimited->has_request_for_fast_response());
  EXPECT_EQ(bytesConsumed, buffer1->size());

  EXPECT_FALSE(delimited->has_slow_response());
}

TEST(ParseDelimited, NullDataTest)
{
  std::shared_ptr<TestTask::Messages::WrapperMessage> delimited;

  size_t bytesConsumed = 0;

  delimited = parseDelimited<TestTask::Messages::WrapperMessage>(
                nullptr,
                0,
                &bytesConsumed
              );

  ASSERT_TRUE(delimited == nullptr);
  EXPECT_EQ(bytesConsumed, 0);
}

TEST(ParseDelimited, EmptyDataTest)
{
  std::shared_ptr<TestTask::Messages::WrapperMessage> delimited;

  size_t bytesConsumed = 0;

  delimited = parseDelimited<TestTask::Messages::WrapperMessage>(
                "",
                0,
                &bytesConsumed
              );

  ASSERT_TRUE(delimited == nullptr);
  EXPECT_EQ(bytesConsumed, 0);
}

TEST(ParseDelimited, WrongDataTest)
{
  std::shared_ptr<TestTask::Messages::WrapperMessage> delimited;

  std::string buffer = "\x05wrong";
  EXPECT_THROW(
    parseDelimited<TestTask::Messages::WrapperMessage>(buffer.data(), buffer.size()),
    std::runtime_error
  );
}

TEST(ParseDelimited, CorruptedDataTest)
{
  std::shared_ptr<TestTask::Messages::WrapperMessage> delimited;

  TestTask::Messages::WrapperMessage message;
  message.mutable_request_for_fast_response();

  auto buffer = serializeDelimited(message);
  size_t bytesConsumed = 0;

  std::string corrupted = std::string(buffer->begin(), buffer->end());
  corrupted[0] -= 1;

  EXPECT_THROW(
    parseDelimited<TestTask::Messages::WrapperMessage>(corrupted.data(), corrupted.size()),
    std::runtime_error
  );
}

TEST(ParseDelimited, WrongMessageSizeTest)
{
  std::shared_ptr<TestTask::Messages::WrapperMessage> delimited;

  TestTask::Messages::WrapperMessage message;
  message.mutable_request_for_fast_response();

  auto buffer = serializeDelimited(message);
  size_t bytesConsumed = 0;

  delimited = parseDelimited<TestTask::Messages::WrapperMessage>(
                buffer->data(),
                buffer->size() * 2,
                &bytesConsumed
              );

  ASSERT_FALSE(delimited == nullptr);
  EXPECT_TRUE(delimited->has_request_for_fast_response());
  EXPECT_EQ(bytesConsumed, buffer->size());

  bytesConsumed = 0;

  delimited = parseDelimited<TestTask::Messages::WrapperMessage>(
                buffer->data(),
                buffer->size() / 2,
                &bytesConsumed
              );

  ASSERT_TRUE(delimited == nullptr);
  EXPECT_EQ(bytesConsumed, 0);
}