/*! \file pg-gstreamer.cpp
 *
 *
 *     File pg-gstreamer.cpp
 *
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */
/* based on ENTEC source code */

#include "pg-all_include.h"

#include <gst/gst.h>

int gstreamer_test(int argc, char* argv[])
{
    GstElement* pipeline;
    GstBus* bus;
    GstMessage* msg;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Build the pipeline */
    pipeline =
        //gst_parse_launch
        //("playbin uri=https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm",
        //    NULL);
    gst_parse_launch
    ("playbin uri=rtsp://192.168.1.65:8554/main",
        NULL);

    /* Start playing */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    /* Wait until error or EOS */
    bus = gst_element_get_bus(pipeline);
    msg =
        gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
            GstMessageType(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    /* See next tutorial for proper error message handling/parsing */
    if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
        g_error("An error occurred! Re-run with the GST_DEBUG=*:WARN environment "
            "variable set for more details.");
    }

    /* Free resources */
    gst_message_unref(msg);
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    return 0;
}
