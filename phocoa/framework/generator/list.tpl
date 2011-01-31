{* vim: set expandtab tabstop=4 shiftwidth=4 syntax=smarty: *}
<h2>{SssSBla value="{{$entityName}}Plur"}</h2>
<p><a href="{WFURL action="edit"}">{SssSBla value="{{$entityName}}AddNew"}.</a></p>

<h3>{SssSBla value="{{$entityName}}Search"}</h3>
<div class="form-container">
{WFViewBlock id="{{$listFormId}}"}
    {WFView id="paginatorState"}
    <fieldset>
        <p><label for="query">Enter a partial {{$descriptiveColumnName}}:</label></p>
		<div>
			{WFView id="query"} {WFView id="search"} {WFView id="clear"}
		</div>
    </fieldset>
{/WFViewBlock}
</div>{* end form-container *}

<p>{WFView id="paginatorPageInfo"} {WFView id="paginatorNavigation"}</p>

<table border="0" cellspacing="0" cellpadding="5" class="datagrid">
{section name=items loop=$__module->valueForKeyPath('{{$sharedEntityId}}.arrangedObjectCount')}
    {if $smarty.section.items.first}
    <tr>
        <th>{SssSBla value="{{$entityName}}Sing"}</th>
        <th></th>
    </tr>
    {/if}
    <tr>
        <td>{WFView id="{{$descriptiveColumnName}}"}</td>
        <td>{WFView id="editLink"} {WFView id="deleteLink"}</td>
    </tr>
{sectionelse}
    <tr><td>{SssSBla value="{{$entityName}}NoneFound"}</td></tr>
{/section}
</table>

<script>
{literal}
Event.observe(window, 'load', function() { document.forms.{{$listFormId}}.query.focus(); });
{/literal}
</script>

