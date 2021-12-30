#include "base/lidig_logger.h"

int main(int argc, char const *argv[])
{
    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(FNLog::PRIORITY_TRACE);
    lidig_logger::get_instance().add_logger_to_file("/tmp", "main", FNLog::PRIORITY_TRACE);
    lidig_logger::get_instance().add_logger_to_udp("127.0.0.1", 7000, FNLog::PRIORITY_TRACE);
    lidig_logger::get_instance().start();

    LogAlarm() << "log init success. shm size:"
        << sizeof(FNLog::SHMLogger) << ", logger size:" << sizeof(FNLog::Logger);
    //FNLog::SetAllChannelCategory(FNLog::GetDefaultLogger(), 100, 1);
    //FNLog::SetAllScreenCategory(FNLog::GetDefaultLogger(), 100, 1);
    LogTrace() << "now time:" << time(nullptr) << ";";
    LogDebug() << "now time:" << time(nullptr) << ";";
    LogInfo() << "now time:" << time(nullptr) << ";";
    LogWarn() << "now time:" << time(nullptr) << ";";
    LogError() << "now time:" << time(nullptr) << ";";
    LogAlarm() << "now time:" << time(nullptr) << ";";
    LogFatal() << "now time:" << time(nullptr) << ";";
    //FNLog::SetAllChannelCategory(FNLog::GetDefaultLogger(), 0, 0);
    //FNLog::SetAllScreenCategory(FNLog::GetDefaultLogger(), 0, 0);
    LOGT("now time:" << time(nullptr) << ";");
    LOGD("now time:" << time(nullptr) << ";");
    LOGI("now time:" << time(nullptr) << ";");
    LOGW("now time:" << time(nullptr) << ";");
    LOGE("now time:" << time(nullptr) << ";");
    LOGA("now time:" << time(nullptr) << ";");
    LOGF("now time:" << time(nullptr) << ";");

    LOGFMTT("now time:<%llu>", (unsigned long long)time(nullptr));
    LOGFMTD("now time:<%llu>", (unsigned long long)time(nullptr));
    LOGFMTI("now time:<%llu>", (unsigned long long)time(nullptr));
    LOGFMTW("now time:<%llu>", (unsigned long long)time(nullptr));
    LOGFMTE("now time:<%llu>", (unsigned long long)time(nullptr));
    LOGFMTA("now time:<%llu>", (unsigned long long)time(nullptr));
    LOGFMTF("now time:<%llu>", (unsigned long long)time(nullptr));


    PackTrace("now time:", time(nullptr), ";");
    PackDebug("now time:", time(nullptr), ";");
    PackInfo("now time:", time(nullptr), ";");
    PackWarn("now time:", time(nullptr), ";");
    PackError("now time:", time(nullptr), ";");
    PackAlarm("now time:", time(nullptr), ";");
    PackFatal("now time:", time(nullptr), ";");

    char data[10] = {1,2,3,4,5};
    LogInfo().write_binary(data, 10);


    LogAlarm() << "finish";

    return 0;
}