/*
  Copyright 2011 David Robillard <http://drobilla.net>
            2012 Jeremy Salwen   <jeremysalwen@gmail.com>
  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/**
   @file soilmm.hpp API for Soilmm, a Suil C++ wrapper library.
*/

#ifndef SOIL_SOILMM_HPP
#define SOIL_SOILMM_HPP

#include <suil/suil.h>
#include <memory>


#define SOIL_DEFINE_CONST_UNWRAP(T) \
	static inline const Suil ## T * ConstUnwrap(const T* node) {return reinterpret_cast<const Suil ## T *>(node); } \

#define SOIL_DEFINE_CONST_WRAP(T) \
	static inline const T* ConstWrap(const Suil ## T * node) {return reinterpret_cast<const T*>(node); } \
	SOIL_DEFINE_CONST_UNWRAP(T)

#define SOIL_DEFINE_UNWRAP(T) \
	static inline Suil ## T * Unwrap(T* node) {return reinterpret_cast<Suil ## T *>(node); } \

#define SOIL_DEFINE_WRAP(T) \
	static inline T* Wrap(Suil ## T * node) {return reinterpret_cast<T*>(node); } \
	SOIL_DEFINE_CONST_WRAP(T)\
	SOIL_DEFINE_UNWRAP(T)

/**
   @defgroup suil Suil
   A library for hosting LV2 plugin UIs.
   @{
*/

namespace Soil {

struct Instance {
	SOIL_DEFINE_WRAP(Instance)

	SuilWidget get_widget() {
		return suil_instance_get_widget(Unwrap(this));
	}
	void port_event(uint32_t port_index, uint32_t buffer_size, uint32_t format, const void *buffer) {
		suil_instance_port_event(Unwrap(this), port_index, buffer_size, format, buffer);
	}
	const void* extension_data(const char* uri) {
		return suil_instance_extension_data(Unwrap(this), uri);
	}
};


struct Host {
	SOIL_DEFINE_WRAP(Host)

	static std::unique_ptr<Host,std::function<void(Host*)>> New(SuilPortWriteFunc write_func, SuilPortIndexFunc index_func, SuilPortSubscribeFunc subscribe_func, SuilPortUnsubscribeFunc unsubscribe_func) {
		auto deleter=[&](Host* n) { suil_host_free(Host::Unwrap(n)); };
		return std::unique_ptr<Host,std::function<void(Host*)>>(
				Wrap(
					suil_host_new(
						write_func,
						index_func,
						subscribe_func,
						unsubscribe_func)),
				deleter);
	}
	
	void set_touch_func(SuilTouchFunc touch_func) {
		suil_host_set_touch_func(Unwrap(this),touch_func);
	}

	std::unique_ptr<Instance,std::function<void(Instance*)>> new_instance(SuilController controller, const char *container_type_uri, const char *plugin_uri, const char *ui_uri, const char *ui_type_uri, const char *ui_bundle_path, const char *ui_binary_path, const LV2_Feature *const *features) {
		auto deleter=[&](Instance* n) { suil_instance_free(Instance::Unwrap(n)); };
		return std::unique_ptr<Instance,std::function<void(Instance*)>>(
			Instance::Wrap(
				suil_instance_new(
					Unwrap(this),
					controller,
					container_type_uri, 
					plugin_uri, 
					ui_uri, 
					ui_type_uri, 
					ui_bundle_path, 
					ui_binary_path, 
					features)),
			deleter);
	}
};

}
#endif /* SOIL_SOILMM_HPP */
