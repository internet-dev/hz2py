/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_hz2py.h"

#include "utf8vector.h"
#include "pinyin.h"

static void php_hz2py(const char *line,
        int line_length,
        int add_blank,
        int polyphone_support,  /** 支持多音字 */
        int first_letter_only,
        int convert_double_char,
        int show_tones,
        zval *result,
        zval *polyphone)
{
    assert(NULL != line);
    assert(NULL != result);
    assert(NULL != polyphone);
    assert(line_length > 0);

    wchar_t uni_char;
    wchar_t last_uni_char = 0;
    const char *utf8;
    int utf8_length;
    size_t SNPRINTF_SIZE = line_length * 8 + 1;
    char *buf = emalloc(SNPRINTF_SIZE);
    char *p = buf;

    memset(buf, 0, SNPRINTF_SIZE);

    utf8vector line_vector = utf8vector_create(line, line_length);

    while ((uni_char = utf8vector_next_unichar_with_raw(line_vector, &utf8, &utf8_length)) != '\0')
    {
        char current[8];
        int uc;
        for (uc = 0; uc < utf8_length && uc < 8; uc++)
        {
            current[uc] = utf8[uc];
        }
        current[uc] = '\0';
        //printf("%s\n", current);

        if (pinyin_ishanzi(uni_char))
        {
            const char **pinyins = NULL;
            int print_count = 0;
            int count = pinyin_get_pinyins_by_unicode(uni_char, &pinyins);
            if (count == 0)
            {
                p += snprintf(p, SNPRINTF_SIZE - (p - buf), "%.*s", utf8_length, utf8);
            }
            else
            {
                char *tones = NULL;
                if (show_tones)
                    pinyin_get_tones_by_unicode(uni_char, &tones);

                // add blank
                if (add_blank && last_uni_char != 0 && !pinyin_ishanzi(last_uni_char))
                {
                    p += snprintf(p, SNPRINTF_SIZE - (p - buf), " ");
                }

                if (first_letter_only)
                {
                    if (show_tones)
                    {
                        if (print_count > 0)
                        {
                            p += snprintf(p, SNPRINTF_SIZE - (p - buf), "|");
                        }

                        p += snprintf(p, SNPRINTF_SIZE - (p - buf), "%c", pinyins[0][0]);
                        print_count ++;
                    }
                    else
                    {
                        p += snprintf(p, SNPRINTF_SIZE - (p - buf), "%c", pinyins[0][0]);
                        print_count++;
                    }
                }
                else
                {
                    if (show_tones)
                    {
                        if (print_count > 0)
                        {
                            p += snprintf(p, SNPRINTF_SIZE - (p - buf), "|");
                        }
                        p += snprintf(p, SNPRINTF_SIZE - (p - buf), "%s", pinyins[0]);
                        print_count ++;
                    }
                    else
                    {
                            p += snprintf(p, SNPRINTF_SIZE - (p - buf), "%s", pinyins[0]);
                            print_count ++;
                    }
                }

                if (show_tones)
                    p += snprintf(p, SNPRINTF_SIZE - (p - buf), "%d", tones[0]);

                do {
                    // 多音字逻辑
                    if (count > 0)
                    {
                        int mi = 0;
                        int has_flag = 0;

                        if (first_letter_only)
                        {
                            char check = pinyins[0][0];
                            for (mi = 1; mi < count; mi++)
                            {
                                if (check != pinyins[mi][0])
                                {
                                    has_flag = 1;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            char *check = (char *)pinyins[0];
                            for (mi = 1; mi < count; mi++)
                            {
                                if (0 != strcmp(pinyins[mi], check))
                                {
                                    has_flag = 1;
                                    break;
                                }
                            }
                        }

                        if (! has_flag)
                        {
                            break;
                        }

                        // 如果已经添加过,测跳过
                        if (zend_hash_exists(Z_ARRVAL_P(polyphone), current, uc))
                        {
                            break;
                        }

                        // 创建字数组
                        zval *subarray;
                        MAKE_STD_ZVAL(subarray);
                        array_init(subarray);

                        // 简拼
                        if (first_letter_only)
                        {
                            char jp[2] = {0};
                            for (mi = 0; mi < count; mi++)
                            {
                                jp[0] = pinyins[mi][0];
                                add_index_string(subarray, mi, jp, 1);
                            }
                        }
                        // 全拼
                        else
                        {
                            for (mi = 0; mi < count; mi++)
                            {
                                add_index_string(subarray, mi, pinyins[mi], 1);
                            }
                        }

                        add_assoc_zval(polyphone, current, subarray);
                    }
                } while (0);

                if (add_blank)
                    p += snprintf(p, SNPRINTF_SIZE - (p - buf), " ");

                if (NULL != tones)
                {
                    free(tones);
                }
            }
            if (NULL != pinyins)
            {
                free(pinyins);
            }
        }
        else
        {
            if (convert_double_char && uni_char > 65280 && uni_char < 65375)
            {
                p += snprintf(p, SNPRINTF_SIZE - (p - buf), "%c", uni_char - 65248);
            }
            else if (convert_double_char && uni_char == 12288)
            {
                p += snprintf(p, SNPRINTF_SIZE - (p - buf), "%c", 32);
            }
            else
            {
                p += snprintf(p, SNPRINTF_SIZE - (p - buf), "%.*s", utf8_length, utf8);
            }
        }
        last_uni_char = uni_char;
    }

    utf8vector_free(line_vector);

    ZVAL_STRINGL(result, buf, p - buf, 1);
    efree(buf);

    return;
}



/* If you declare any globals in php_hz2py.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(hz2py)
*/

/* True global resources - no need for thread safety here */
static int le_hz2py;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("hz2py.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_hz2py_globals, hz2py_globals)
    STD_PHP_INI_ENTRY("hz2py.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_hz2py_globals, hz2py_globals)
PHP_INI_END()
*/
/* }}} */

/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/

/** 包裹函数 */
PHP_FUNCTION(hz2py_jp)
{
    char *src;
    size_t src_len = 0;

    zval *result;
    zval *polyphone;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szz", &src, &src_len, &result, &polyphone) == FAILURE)
    {
        RETVAL_LONG(HZ2PY_ERROR);
        goto END;
    }

    if (0 == src_len)
    {
        RETVAL_LONG(HZ2PY_EMPTY);
        goto END;
    }

    ZVAL_EMPTY_STRING(result);
    if (Z_TYPE_P(polyphone) != IS_ARRAY)
    {
        //php_printf("不是数组,需要初始化");
        array_init(polyphone);
    }

    /** debug {{{ */
    // 需要强制分离
    //ZVAL_STRINGL(result, "abc", 3, 1);
    //add_assoc_string(polyphone, "key", "value", 1);

    //char *str;
    //size_t len = spprintf(&str, 0, "%s", "为后续开发准备");
    //ZVAL_STRINGL(result, str, len, 1);
    /** end of }}} */
    php_hz2py(src, src_len, 0, 1, 1, 0, 0, result, polyphone);

    RETVAL_LONG(HZ2PY_SUCCESS);

END:
    return;
}

PHP_FUNCTION(hz2py_qp)
{
    char *src;
    size_t src_len = 0;

    zval *result;
    zval *polyphone;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szz", &src, &src_len, &result, &polyphone) == FAILURE)
    {
        RETVAL_LONG(HZ2PY_ERROR);
        goto END;
    }

    if (0 == src_len)
    {
        RETVAL_LONG(HZ2PY_EMPTY);
        goto END;
    }

    ZVAL_EMPTY_STRING(result);
    if (Z_TYPE_P(polyphone) != IS_ARRAY)
    {
        array_init(polyphone);
    }

    php_hz2py(src, src_len, 0, 1, 0, 0, 0, result, polyphone);

    RETVAL_LONG(HZ2PY_SUCCESS);

END:

    return;
}

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_hz2py_jp, 0, 0, 3)
    ZEND_ARG_INFO(0, src)
    ZEND_ARG_INFO(1, result)
    ZEND_ARG_INFO(1, polyphone) /* array */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hz2py_qp, 0, 0, 3)
    ZEND_ARG_INFO(0, src)
    ZEND_ARG_INFO(1, result)
    ZEND_ARG_INFO(1, polyphone) /* array */
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ php_hz2py_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_hz2py_init_globals(zend_hz2py_globals *hz2py_globals)
{
    hz2py_globals->global_value = 0;
    hz2py_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(hz2py)
{
    /* If you have INI entries, uncomment these lines
    REGISTER_INI_ENTRIES();
    */
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(hz2py)
{
    /* uncomment this line if you have INI entries
    UNREGISTER_INI_ENTRIES();
    */
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(hz2py)
{
#if defined(COMPILE_DL_HZ2PY) && defined(ZTS)
    ZEND_TSRMLS_CACHE_UPDATE;
#endif
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(hz2py)
{
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(hz2py)
{
    php_info_print_table_start();
    //php_info_print_table_header(2, "hz2py support", "enabled");
    php_info_print_table_row(2, "hz2py support", "enabled");
    php_info_print_table_row(2, "Version", "0.0.1");
    php_info_print_table_row(2, "Charset", "utf-8");
    php_info_print_table_row(2, "Author", "hy0kl[email: hy0kle@gmail.com]");
    php_info_print_table_end();

    /* Remove comments if you have entries in php.ini
    DISPLAY_INI_ENTRIES();
    */
}
/* }}} */

/* {{{ hz2py_functions[]
 *
 * Every user visible function must have an entry in hz2py_functions[].
 */
const zend_function_entry hz2py_functions[] = {
    PHP_FE(hz2py_jp, arginfo_hz2py_jp)
    PHP_FE(hz2py_qp, arginfo_hz2py_qp)
    PHP_FE_END    /* Must be the last line in hz2py_functions[] */
};
/* }}} */

/* {{{ hz2py_module_entry
 */
zend_module_entry hz2py_module_entry = {
    STANDARD_MODULE_HEADER,
    "hz2py",
    hz2py_functions,
    PHP_MINIT(hz2py),
    PHP_MSHUTDOWN(hz2py),
    PHP_RINIT(hz2py),        /* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(hz2py),    /* Replace with NULL if there's nothing to do at request end */
    PHP_MINFO(hz2py),
    PHP_HZ2PY_VERSION,
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_HZ2PY
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE;
#endif
ZEND_GET_MODULE(hz2py)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
