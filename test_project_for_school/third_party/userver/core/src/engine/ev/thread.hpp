#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include <ev.h>

#include <userver/engine/deadline.hpp>

#include <concurrent/impl/intrusive_mpsc_queue.hpp>
#include <engine/ev/async_payload_base.hpp>
#include <engine/ev/event_loop.hpp>
#include <utils/statistics/thread_statistics.hpp>

USERVER_NAMESPACE_BEGIN

namespace engine::ev {

class Thread final {
 public:
  struct UseDefaultEvLoop {};
  static constexpr UseDefaultEvLoop kUseDefaultEvLoop{};

  enum class RegisterEventMode {
    // With this mode RegisterEventInEvLoop will notify ev-loop right away,
    // behaving exactly as RunInEvLoopAsync.
    kImmediate,
    // With this mode RegisterEventInEvLoop will defer events execution to
    // a periodic timer, running with ~1ms resolution. It helps to avoid
    // the ev_async_send call, which incurs very noticeable overhead, however
    // event execution becomes delayed for a aforementioned ~1ms.
    kDeferred
  };

  Thread(const std::string& thread_name, RegisterEventMode);
  Thread(const std::string& thread_name, UseDefaultEvLoop, RegisterEventMode);

  ~Thread();

  struct ev_loop* GetEvLoop() const { return event_loop_.GetEvLoop(); }

  // Callbacks passed to RunInEvLoopAsync() are serialized.
  // All callbacks are guaranteed to execute.
  void RunInEvLoopAsync(AsyncPayloadBase& payload) noexcept;

  // Callbacks passed to RunInEvLoopDeferred() are serialized.
  // Same as RunInEvLoopAsync but doesn't force the wakeup of ev-loop, adding
  // delay up to ~1ms.
  void RunInEvLoopDeferred(AsyncPayloadBase& payload,
                           Deadline deadline) noexcept;

  bool IsInEvThread() const;

  std::uint8_t GetCurrentLoadPercent() const;
  const std::string& GetName() const;

 private:
  Thread(const std::string& thread_name, EventLoop::EvLoopType ev_loop_type,
         RegisterEventMode register_event_mode);

  void RegisterInEvLoop(AsyncPayloadBase& payload);

  void Start();

  void StopEventLoop();
  void RunEvLoop();

  static void UpdateLoopWatcher(struct ev_loop*, ev_async* w, int) noexcept;
  static void UpdateTimersWatcher(struct ev_loop*, ev_timer* w, int) noexcept;
  void UpdateLoopWatcherImpl();
  static void BreakLoopWatcher(struct ev_loop*, ev_async* w, int) noexcept;
  void BreakLoopWatcherImpl();

  static void Acquire(struct ev_loop* loop) noexcept;
  static void Release(struct ev_loop* loop) noexcept;
  void AcquireImpl() noexcept;
  void ReleaseImpl() noexcept;

  concurrent::impl::IntrusiveMpscQueue<AsyncPayloadBase> func_queue_{};

  RegisterEventMode register_event_mode_;

  EventLoop event_loop_;

  std::thread thread_{};
  std::mutex loop_mutex_{};
  std::unique_lock<std::mutex> lock_{loop_mutex_, std::defer_lock};

  ev_timer timers_driver_{};
  ev_timer stats_timer_{};
  ev_async watch_update_{};
  ev_async watch_break_{};

  const std::string name_;
  utils::statistics::ThreadCpuStatsStorage cpu_stats_storage_;
  bool is_running_{false};
};

}  // namespace engine::ev

USERVER_NAMESPACE_END