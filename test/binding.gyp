{
  'target_defaults': {
    'includes': ['../common.gypi'],
    'include_dirs': ['./common'],
    'variables': {
      'build_sources': [
        'addon.cc',
        'addon_data.cc',
        'array_buffer.cc',
        'async_context.cc',
        'async_progress_queue_worker.cc',
        'async_progress_worker.cc',
        'async_worker.cc',
        'async_worker_persistent.cc',
        'basic_types/array.cc',
        'basic_types/boolean.cc',
        'basic_types/number.cc',
        'basic_types/value.cc',
        'bigint.cc',
        'date.cc',
        'binding.cc',
        'buffer.cc',
        'callbackscope.cc',
        'dataview/dataview.cc',
        'dataview/dataview_read_write.cc',
        'env_cleanup.cc',
        'error.cc',
        'errorHandlingForPrimitives.cc',
        'external.cc',
        'function.cc',
        'function_reference.cc',
        'handlescope.cc',
        'maybe/check.cc',
        'movable_callbacks.cc',
        'memory_management.cc',
        'name.cc',
        'globalObject/global_object_delete_property.cc',
        'globalObject/global_object_has_own_property.cc',
        'globalObject/global_object_set_property.cc',
        'globalObject/global_object_get_property.cc',
        'globalObject/global_object.cc',
        'object/delete_property.cc',
        'object/finalizer.cc',
        'object/get_property.cc',
        'object/has_own_property.cc',
        'object/has_property.cc',
        'object/object.cc',
        'object/object_freeze_seal.cc',
        'object/set_property.cc',
        'object/subscript_operator.cc',
        'promise.cc',
        'run_script.cc',
        "symbol.cc",
        'threadsafe_function/threadsafe_function_ctx.cc',
        'threadsafe_function/threadsafe_function_existing_tsfn.cc',
        'threadsafe_function/threadsafe_function_ptr.cc',
        'threadsafe_function/threadsafe_function_sum.cc',
        'threadsafe_function/threadsafe_function_unref.cc',
        'threadsafe_function/threadsafe_function.cc',
        'typed_threadsafe_function/typed_threadsafe_function_ctx.cc',
        'typed_threadsafe_function/typed_threadsafe_function_existing_tsfn.cc',
        'typed_threadsafe_function/typed_threadsafe_function_ptr.cc',
        'typed_threadsafe_function/typed_threadsafe_function_sum.cc',
        'typed_threadsafe_function/typed_threadsafe_function_unref.cc',
        'typed_threadsafe_function/typed_threadsafe_function.cc',
        'typedarray.cc',
        'objectwrap.cc',
        'objectwrap_constructor_exception.cc',
        'objectwrap_removewrap.cc',
        'objectwrap_multiple_inheritance.cc',
        'object_reference.cc',
        'reference.cc',
        'version_management.cc',
        'thunking_manual.cc',
      ],
      'build_sources_swallowexcept': [
        'binding-swallowexcept.cc',
        'error.cc',
      ],
      'conditions': [
        ['disable_deprecated!="true"', {
          'build_sources': ['object/object_deprecated.cc']
        }]
      ]
    },
  },
  'targets': [
    {
      'target_name': 'binding',
      'includes': ['../except.gypi'],
      'sources': ['>@(build_sources)']
    },
    {
      'target_name': 'binding_noexcept',
      'includes': ['../noexcept.gypi'],
      'sources': ['>@(build_sources)']
    },
    {
      'target_name': 'binding_noexcept_maybe',
      'includes': ['../noexcept.gypi'],
      'sources': ['>@(build_sources)'],
      'defines': ['NODE_ADDON_API_ENABLE_MAYBE']
    },
    {
      'target_name': 'binding_swallowexcept',
      'includes': ['../except.gypi'],
      'sources': [ '>@(build_sources_swallowexcept)'],
      'defines': ['NODE_API_SWALLOW_UNTHROWABLE_EXCEPTIONS']
    },
    {
      'target_name': 'binding_swallowexcept_noexcept',
      'includes': ['../noexcept.gypi'],
      'sources': ['>@(build_sources_swallowexcept)'],
      'defines': ['NODE_API_SWALLOW_UNTHROWABLE_EXCEPTIONS']
    },
  ],
}
