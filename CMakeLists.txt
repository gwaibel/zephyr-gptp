cmake_minimum_required(VERSION 3.15)

project(gptp)


add_definitions(
   -DCONFIG_NET_GPTP
   -DCONFIG_NET_GPTP_PATH_TRACE_ELEMENTS=10
   -DCONFIG_NET_GPTP_NUM_PORTS=5
   -DCONFIG_NET_GPTP_CLOCK_ACCURACY=100
   -DCONFIG_NET_GPTP_INIT_LOG_SYNC_ITV=100
   -DCONFIG_NET_GPTP_INIT_LOG_ANNOUNCE_ITV=100
   -DCONFIG_NET_GPTP_ANNOUNCE_RECEIPT_TIMEOUT=100
   -DCONFIG_NET_GPTP_SYNC_RECEIPT_TIMEOUT=100
   -DCONFIG_NET_GPTP_INIT_LOG_PDELAY_REQ_ITV=100
   -DCONFIG_NET_PKT_TIMESTAMP
   -DCONFIG_NET_TC_TX_COUNT=3
   -DCONFIG_NET_TC_RX_COUNT=3
#   -DCONFIG_NET_GPTP_GM_CAPABLE
   -DCONFIG_NET_GPTP_LOG_LEVEL=4   # 0=NONE, 1=ERR, 2=WRN, 3=INF, 4=DBG
   -DCONFIG_NET_GPTP_USE_DEFAULT_CLOCK_UPDATE
   -DCONFIG_NET_GPTP_NETPKT_COUNT=10  # Number of TX and RX net packet buffers required at the same time

)

add_executable(gptp)
target_sources(gptp
   PRIVATE
      gptp/gptp.c
      gptp/gptp_md.c
      gptp/gptp_messages.c
      gptp/gptp_mi.c
      gptp/gptp_user_api.c
      target/gptp_trg_sys.c
      target/gptp_trg_timer.c
      target/gptp_trg_thread.c
#      target/gptp_trg_net.c
      target/gptp_trg_ptp.c
#      main.c
)
target_include_directories(gptp
   PRIVATE
      include
      gptp
      target
)
target_link_libraries(gptp
   PRIVATE
      pthread
)

