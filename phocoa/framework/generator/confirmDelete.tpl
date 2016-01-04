<h2>{SssSBla value="{{$entityName}}Sing"}</h2>

<div class="form-container">
{WFViewBlock id="{{$confirmDeleteFormId}}"}
    {WFView id="{{$entity->valueForKey('primaryKeyProperty')}}"}
    {WFView id="confirmMessage"}

    <div class="buttonrow">
        {WFView id="cancel"}{WFView id="deleteObj"}
    </div>
{/WFViewBlock}
</div>{* end form-container *}
<a href="{{$modulePath}}/list">{SssSBla value="SharedBack2list"}</a>

