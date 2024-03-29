<?php

/**
 * @file
 * Contains the FooTable style plugin.
 */

/**
 * Style plugin to render a FooTable.
 *
 * @ingroup views_style_plugins
 */
class footable_plugin_style_footable extends views_plugin_style_table {

  /**
   * {@inheritdoc}
   */
  function option_definition() {
    $options = parent::option_definition();
    $options['footable'] = [
      'default' => [
        'expand_all' => FALSE,
        'expand_first' => FALSE,
        'show_header' => TRUE,
        'toggle_column' => 'first',
        'icon_expanded' => 'plus',
        'icon_collapsed' => 'minus',
        'bootstrap' => [
          'striped' => FALSE,
          'bordered' => FALSE,
          'hover' => FALSE,
          'condensed' => FALSE,
        ],
        'component' => [
          'paging' => [
            'enabled' => FALSE,
            'count_format' => '{CP} of {TP}',
            'current' => 1,
            'limit' => 5,
            'position' => 'right',
            'size' => 10,
          ],
          'filtering' => [
            'enabled' => FALSE,
            'delay' => 1200,
            'ignore_case' => TRUE,
            'min' => 3,
            'placeholder' => 'Search',
            'position' => 'right',
            'space' => 'AND',
          ],
          'sorting' => [
            'enabled' => FALSE,
          ],
        ],
      ],
    ];
    return $options;
  }

  /**
   * {@inheritdoc}
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    // Check if fields have been added.  Table style plugin will set
    // error_markup if fields are not added.
    // @see views_plugin_style_table::options_form()
    if (isset($form['error_markup'])) {
      return;
    }

    // Override default plugin theme.
    $form['#theme'] = 'footable_style_plugin_table';

    $columns = $this->sanitize_columns($this->options['columns']);
    foreach ($columns as $field => $column) {
      $form['info'][$field]['type'] = [
        '#type' => 'select',
        '#options' => [
          '' => t('Automatic'),
          'number' => t('Number'),
          'date' => t('Date'),
          'text' => t('Text'),
          'html' => t('HTML'),
        ],
        '#default_value' => !empty($this->options['info'][$field]['type']) ? $this->options['info'][$field]['type'] : '',
        '#states' => [
          'visible' => [
            '#edit-style-options-columns-' . $field => ['value' => $field],
          ],
        ],
      ];
    }

    $form['footable'] = [
      '#type' => 'fieldset',
      '#title' => t('FooTable settings'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    ];

    $form['footable']['expand_all'] = [
      '#type' => 'select',
      '#title' => t('Expand all rows'),
      '#description' => t('Whether or not to expand all rows of the table.'),
      '#options' => [
        FALSE => t('Disabled'),
        TRUE => t('Enabled'),
      ],
      '#default_value' => $this->options['footable']['expand_all'],
    ];

    $form['footable']['expand_first'] = [
      '#type' => 'select',
      '#title' => t('Expand first row'),
      '#description' => t('Whether or not to expand the first row of the table.'),
      '#options' => [
        FALSE => t('Disabled'),
        TRUE => t('Enabled'),
      ],
      '#default_value' => $this->options['footable']['expand_first'],
      '#states' => [
        'invisible' => [
          ':input[name="style_options[footable][expand_all]"]' => ['value' => 1],
        ],
      ],
    ];

    $form['footable']['show_header'] = [
      '#type' => 'select',
      '#title' => t('Show header'),
      '#description' => t('Whether or not to display a header row in the table.'),
      '#options' => [
        TRUE => t('Yes'),
        FALSE => t('No'),
      ],
      '#default_value' => $this->options['footable']['show_header'],
    ];

    $form['footable']['toggle_column'] = [
      '#type' => 'select',
      '#title' => t('Expandable column'),
      '#description' => t('Specify which column the toggle is appended to in a row.'),
      '#options' => [
        'first' => t('First'),
        'last' => t('Last'),
      ],
      '#default_value' => $this->options['footable']['toggle_column'],
    ];

    // Icons.
    $icons = footable_icons();
    $form['footable']['icon_collapsed'] = [
      '#type' => 'select',
      '#title' => t('Collapsed icon style'),
      '#description' => t('The icon displayed when a row is collapsed.'),
      '#options' => $icons,
      '#default_value' => $this->options['footable']['icon_collapsed'],
    ];

    $form['footable']['icon_expanded'] = [
      '#type' => 'select',
      '#title' => t('Expanded icon style'),
      '#description' => t('The icon displayed when a row is expanded.'),
      '#options' => $icons,
      '#default_value' => $this->options['footable']['icon_expanded'],
    ];

    // Bootstrap style configuration.
    if (config_get('footable.settings', 'footable_plugin_type') == 'bootstrap') {
      $form['footable']['bootstrap'] = [
        '#type' => 'fieldset',
        '#title' => t('Bootstrap'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      ];

      $form['footable']['bootstrap']['striped'] = [
        '#type' => 'checkbox',
        '#title' => t('Striped'),
        '#default_value' => $this->options['footable']['bootstrap']['striped'],
      ];

      $form['footable']['bootstrap']['bordered'] = [
        '#type' => 'checkbox',
        '#title' => t('Bordered'),
        '#default_value' => $this->options['footable']['bootstrap']['bordered'],
      ];

      $form['footable']['bootstrap']['hover'] = [
        '#type' => 'checkbox',
        '#title' => t('Hover'),
        '#default_value' => $this->options['footable']['bootstrap']['hover'],
      ];

      $form['footable']['bootstrap']['condensed'] = [
        '#type' => 'checkbox',
        '#title' => t('Condensed'),
        '#default_value' => $this->options['footable']['bootstrap']['condensed'],
      ];
    }

    // Components.
    $form['footable']['component'] = [
      '#type' => 'fieldset',
      '#title' => t('Components'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    ];

    // Filtering.
    $form['footable']['component']['filtering'] = [
      '#type' => 'fieldset',
      '#title' => t('Filtering'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    ];

    $form['footable']['component']['filtering']['enabled'] = [
      '#type' => 'checkbox',
      '#title' => t('Enabled'),
      '#default_value' => $this->options['footable']['component']['filtering']['enabled'],
    ];

    $form['footable']['component']['filtering']['delay'] = [
      '#type' => 'number',
      '#title' => t('Delay'),
      '#description' => t('The number of milliseconds before a search input filter is applied after it changes.'),
      '#default_value' => $this->options['footable']['component']['filtering']['delay'],
      '#min' => 0,
      '#states' => [
        'visible' => [
          ':input[name="style_options[footable][component][filtering][enabled]"]' => ['checked' => TRUE],
        ],
      ],
    ];

    $form['footable']['component']['filtering']['ignore_case'] = [
      '#type' => 'select',
      '#title' => t('Ignore case'),
      '#description' => t('Whether or not to ignore case when filtering.'),
      '#options' => [
        TRUE => t('Yes'),
        FALSE => t('No'),
      ],
      '#default_value' => $this->options['footable']['component']['filtering']['ignore_case'],
      '#states' => [
        'visible' => [
          ':input[name="style_options[footable][component][filtering][enabled]"]' => ['checked' => TRUE],
        ],
      ],
    ];

    $form['footable']['component']['filtering']['min'] = [
      '#type' => 'number',
      '#title' => t('Minimum characters'),
      '#description' => t('The minimum number of characters in the search input before auto applying the filter.'),
      '#default_value' => $this->options['footable']['component']['filtering']['min'],
      '#min' => 0,
      '#states' => [
        'visible' => [
          ':input[name="style_options[footable][component][filtering][enabled]"]' => ['checked' => TRUE],
        ],
      ],
    ];

    $form['footable']['component']['filtering']['placeholder'] = [
      '#type' => 'textfield',
      '#title' => t('Placeholder'),
      '#description' => t('The placeholder text displayed within the search input.'),
      '#default_value' => $this->options['footable']['component']['filtering']['placeholder'],
      '#states' => [
        'visible' => [
          ':input[name="style_options[footable][component][filtering][enabled]"]' => ['checked' => TRUE],
        ],
      ],
    ];

    $form['footable']['component']['filtering']['position'] = [
      '#type' => 'select',
      '#title' => t('Position'),
      '#description' => t('The position of the search input within the filter row.'),
      '#options' => [
        'right' => t('Right'),
        'left' => t('Left'),
        'center' => t('Center'),
      ],
      '#default_value' => $this->options['footable']['component']['filtering']['position'],
      '#states' => [
        'visible' => [
          ':input[name="style_options[footable][component][filtering][enabled]"]' => ['checked' => TRUE],
        ],
      ],
    ];

    $form['footable']['component']['filtering']['space'] = [
      '#type' => 'select',
      '#title' => t('Space'),
      '#description' => t('How to treat whitespace.'),
      '#options' => [
        'AND' => 'AND',
        'OR' => 'OR',
      ],
      '#default_value' => $this->options['footable']['component']['filtering']['space'],
      '#states' => [
        'visible' => [
          ':input[name="style_options[footable][component][filtering][enabled]"]' => ['checked' => TRUE],
        ],
      ],
    ];

    // Paging.
    $form['footable']['component']['paging'] = [
      '#type' => 'fieldset',
      '#title' => t('Paging'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    ];

    $form['footable']['component']['paging']['enabled'] = [
      '#type' => 'checkbox',
      '#title' => t('Enabled'),
      '#default_value' => $this->options['footable']['component']['paging']['enabled'],
    ];

    $form['footable']['component']['paging']['count_format'] = [
      '#type' => 'textfield',
      '#title' => t('Count format'),
      '#description' => t('The string used as a format to generate the count text.'),
      '#default_value' => $this->options['footable']['component']['paging']['count_format'],
      '#states' => [
        'visible' => [
          ':input[name="style_options[footable][component][paging][enabled]"]' => ['checked' => TRUE],
        ],
      ],
    ];

    $form['footable']['component']['paging']['current'] = [
      '#type' => 'number',
      '#title' => t('Current'),
      '#description' => t('The page number to display when first initialized.'),
      '#default_value' => $this->options['footable']['component']['paging']['current'],
      '#min' => 0,
      '#states' => [
        'visible' => [
          ':input[name="style_options[footable][component][paging][enabled]"]' => ['checked' => TRUE],
        ],
      ],
    ];

    $form['footable']['component']['paging']['limit'] = [
      '#type' => 'number',
      '#title' => t('Limit'),
      '#description' => t('The maximum number of page links to display in the pagination control.'),
      '#default_value' => $this->options['footable']['component']['paging']['limit'],
      '#min' => 0,
      '#states' => [
        'visible' => [
          ':input[name="style_options[footable][component][paging][enabled]"]' => ['checked' => TRUE],
        ],
      ],
    ];

    $form['footable']['component']['paging']['position'] = [
      '#type' => 'select',
      '#title' => t('Position'),
      '#description' => t('The position of the pagination control within the paging row.'),
      '#options' => [
        'right' => t('Right'),
        'left' => t('Left'),
        'center' => t('Center'),
      ],
      '#default_value' => $this->options['footable']['component']['paging']['position'],
      '#states' => [
        'visible' => [
          ':input[name="style_options[footable][component][paging][enabled]"]' => ['checked' => TRUE],
        ],
      ],
    ];

    $form['footable']['component']['paging']['size'] = [
      '#type' => 'number',
      '#title' => t('Size'),
      '#description' => t('The number of rows per page.'),
      '#default_value' => $this->options['footable']['component']['paging']['size'],
      '#min' => 0,
      '#states' => [
        'visible' => [
          ':input[name="style_options[footable][component][paging][enabled]"]' => ['checked' => TRUE],
        ],
      ],
    ];

    // Sorting.
    $form['footable']['component']['sorting'] = [
      '#type' => 'fieldset',
      '#title' => t('Sorting'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    ];

    $form['footable']['component']['sorting']['enabled'] = [
      '#type' => 'checkbox',
      '#title' => t('Enabled'),
      '#default_value' => $this->options['footable']['component']['sorting']['enabled'],
    ];

    // Breakpoints overwrite.
    $form['footable']['overwrite'] = [
      '#type' => 'fieldset',
      '#title' => t('Overwrite breakpoints'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    ];

    foreach (footable_breakpoint_load_multiple() as $breakpoint) {
      $form['footable']['overwrite'][$breakpoint->machine_name] = [
        '#type' => 'number',
        '#title' => check_plain($breakpoint->name),
        '#default_value' => isset($this->options['footable']['overwrite'][$breakpoint->machine_name]) ? $this->options['footable']['overwrite'][$breakpoint->machine_name] : NULL,
        '#min' => 0,
        '#field_suffix' => 'px',
        '#attributes' => [
          'placeholder' => $breakpoint->breakpoint,
        ],
      ];
    }

    // Breakpoints.
    $form['footable']['breakpoint'] = [
      '#type' => 'fieldset',
      '#title' => t('Collapsed columns'),
      '#description' => t('Select the "breakpoints" where a particular column should be hidden.'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    ];

    $breakpoints = [];
    foreach (footable_breakpoint_load_all(TRUE) as $breakpoint) {
      $breakpoints[$breakpoint->machine_name] = check_plain($breakpoint->name);
    }

    foreach ($this->display->handler->get_field_labels() as $name => $label) {
      $form['footable']['breakpoint'][$name] = [
        '#title' => check_plain($label),
        '#type' => 'checkboxes',
        '#options' => $breakpoints,
        '#default_value' => isset($this->options['footable']['breakpoint'][$name]) ? $this->options['footable']['breakpoint'][$name] : [],
        '#multiple' => TRUE,
      ];
    }
  }

}
