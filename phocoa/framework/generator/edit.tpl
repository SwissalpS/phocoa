{* vim: set expandtab tabstop=4 shiftwidth=4 syntax=smarty: *}
<h2>{SssSBla value="{{$entityName}}Sing"}</h2>
<div class="form-container">
{WFView id="statusMessage"}
{WFShowErrors id="{{$editFormId}}"}

{WFViewBlock id="{{$editFormId}}"}
	<fieldset>
		<legend>{SssSBla value="{{$entityName}}Detail"}</legend>
    {{foreach name=widgets from=$widgets key="widgetId" item="property"}}
        {{if $widgetId == $entity->valueForKey('primaryKeyProperty')}}
{WFViewHiddenHelper id="{{$widgetId}}"}{WFView id="{{$widgetId}}"}{/WFViewHiddenHelper}
		{{elseif $widgetId == $entityNewWidgetID}}
{WFViewHiddenHelper id="{{$widgetId}}"}
		<div>
			<label for="{{$widgetId}}">{SssSBla value="{{$entityName}}{{$property->valueForKey('name')}}"}:</label>
			{WFView id="{{$widgetId}}"}{WFShowErrors id="{{$widgetId}}"}
		</div>
			{/WFViewHiddenHelper}
		{{else}}
<div>
			<label for="{{$widgetId}}">{SssSBla value="{{$entityName}}{{$property->valueForKey('name')}}"}:</label>
			{WFView id="{{$widgetId}}"}{WFShowErrors id="{{$widgetId}}"}
		</div>
        {{/if}}
    {{/foreach}}
		<div class="buttonrow">
			{WFView id="saveNew"}{WFView id="save"}{WFView id="deleteObj"}
		</div>
	</fieldset>
{/WFViewBlock}
</div>{* end form-container *}
<a href="{{$modulePath}}/list">{SssSBla value="SharedBack2list"}</a>
